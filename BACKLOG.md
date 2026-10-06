# cpp-tts — Implementation Backlog

Ticketed backlog derived from `DESIGN.md`. Each ticket: **track**, **depends on**, what to build, and what "done" looks like. Ticket numbers are sequential but not strictly a required build order within a phase — items in the same phase without a dependency on each other can be done in any order. Phases map to `DESIGN.md` Section 31's M0–M18.

Format: `TTS-###: Title` — *(track; depends: …)* description. **Done when:** acceptance check.

---

## Phase 0 — Repo & Tooling

- **TTS-001: CMake project skeleton** — *(infra; depends: none)* Set up the `cpp-tts/` directory tree from Section 28 (`engine/`, `audio/`, `text/`, `models/`, `dataset/`, `training/`, `tools/`, `tests/`, `examples/`) with a root `CMakeLists.txt` and per-module subdirectories. **Done when:** `cmake --build` succeeds on an empty skeleton with one placeholder source file per module.
- **TTS-002: ctest integration** — *(infra; depends: 001)* Wire `tests/` into CMake's `ctest` so every numerical test (FFT identity, gradient checks, overfit tests) runs with one command. **Done when:** `ctest` runs and reports pass/fail for at least one dummy test.
- **TTS-003: Third-party dependency policy in CMake** — *(infra; depends: 001)* Add `FetchContent` or vendored setup for the *validation-only* deps allowed by Section 32 (FFTW for benchmarking, an image-writing lib, a G2P library) — none linked into the training/inference path. **Done when:** each optional dep is behind a CMake option flag, off by default except for tests that need it.

---

## Phase 1 — Audio I/O and DFT/FFT (Track B, milestones M1–M4)

- **TTS-004: WavFile reader** — *(audio; depends: 001)* Hand-written RIFF/WAVE parser: read fmt/data chunks, support 16-bit PCM mono, expose sample rate, bit depth, channel count, sample buffer as `std::vector<float>` (÷32768 normalization). **Done when:** reads a real WAV file and reports correct metadata (M1).
- **TTS-005: WavFile writer** — *(audio; depends: 004)* Inverse of the reader — write a valid RIFF header + PCM data from a float buffer. **Done when:** round-trip read→write→read is byte-identical.
- **TTS-006: WAV round-trip test** — *(audio; depends: 004, 005)* Automated ctest asserting byte-identical round trip on 2–3 sample files. **Done when:** test passes in CI/ctest (completes M1).
- **TTS-007: Complex number type** — *(audio; depends: 001)* Either wrap `std::complex<float>` or implement your own with +,-,*,/, conjugate, magnitude, phase. **Done when:** unit tests for each operator pass.
- **TTS-008: Naive DFT / IDFT** — *(audio; depends: 007)* O(N²) forward and inverse DFT per the Section 4 equation — this is your ground truth for everything downstream. **Done when:** `IDFT(DFT(x)) ≈ x` at 1e-5 tolerance on random and pure-sinusoid inputs (M2).
- **TTS-009: Radix-2 FFT** — *(audio; depends: 008)* Cooley-Tukey FFT (recursive or iterative with bit-reversal), power-of-2 length required; add zero-padding helper for arbitrary lengths. **Done when:** `FFT(x) ≈ DFT(x)` elementwise at N ∈ {64,256,1024,4096} (M3).
- **TTS-010: Inverse FFT** — *(audio; depends: 009)* Implement via the conjugate trick or directly. **Done when:** `IFFT(FFT(x)) ≈ x` at 1e-5 tolerance (M4).
- **TTS-011: FFT correctness/benchmark suite** — *(audio; depends: 009, 010)* Known-sinusoid bin test, Parseval energy-conservation test, linearity test, and a DFT-vs-FFT runtime benchmark plotted log-log. **Done when:** all correctness tests pass and the benchmark visibly shows O(N log N) vs O(N²) scaling.

---

## Phase 2 — STFT and Spectrograms (Track B, M5–M6)

- **TTS-012: Window functions** — *(audio; depends: 007)* Hann and Hamming window generators. **Done when:** output values match the closed-form formulas at a handful of sample points.
- **TTS-013: Framing utility** — *(audio; depends: 012)* Slice a signal into overlapping frames given `fft_size`, `hop_size`, with center-padding (reflect or zero). **Done when:** frame count matches `1 + floor((len + 2·pad − fft_size)/hop_size)`.
- **TTS-014: STFT** — *(audio; depends: 009, 012, 013)* `stft(signal, fft_size, hop_size, window)` → `[num_frames, num_freq_bins]` complex tensor (keep only the non-redundant half of the spectrum). **Done when:** shape matches the formula and output validated against TTS-008/009 on a synthetic signal.
- **TTS-015: ISTFT + overlap-add** — *(audio; depends: 014)* Reconstruct a signal from its STFT via per-frame IFFT + windowed overlap-add, normalized by the sum-of-squared-window divisor. **Done when:** `ISTFT(STFT(x)) ≈ x` at negligible error (M5).
- **TTS-016: Spectrogram utilities** — *(audio; depends: 014)* Magnitude, power, and log-magnitude (dB, epsilon-floored) conversions from a complex STFT. **Done when:** each conversion has a unit test checking known input→output values, including the epsilon floor preventing `log(0)`.
- **TTS-017: Spectrogram/waveform viewer tool** — *(infra/audio; depends: 016)* `tools/wav_inspect` + `tools/spectrogram` CLI: report WAV metadata, render waveform and log-magnitude spectrogram as images. **Done when:** output visually cross-checks against a reference tool (e.g. librosa, used only for comparison) on the same file/params (M6).
- **TTS-018: Own-voice spectrogram gallery** — *(audio; depends: 017)* Record and label spectrograms for: sustained vowel, voiced consonant, unvoiced fricative, plosive, silence, rising/falling pitch. **Done when:** you can point to each phenomenon's visual signature (harmonic stripes, broadband noise, silence, F0 movement) on your own recordings.

---

## Phase 3 — Mel Spectrogram and Reconstruction (Track B, M7–M8)

- **TTS-019: Hz↔mel conversion** — *(audio; depends: 001)* `hz_to_mel` / `mel_to_hz` per Section 8's formulas. **Done when:** round-trips within numerical tolerance and matches known reference values.
- **TTS-020: MelFilterBank** — *(audio; depends: 019, 014)* Construct the `[num_mels, n_fft/2+1]` triangular filterbank matrix given `n_fft`, `n_mels`, `sample_rate`, `fmin`, `fmax`. **Done when:** filter shape/overlap properties checked by unit test (each filter ramps 0→1→0, filters overlap at half-height).
- **TTS-021: MelSpectrogram pipeline** — *(audio; depends: 016, 020)* `MelSpectrogram::compute(signal)` → `[num_frames, num_mels]` log-mel tensor; fixed, documented epsilon floor and normalization constants (mean/std recorded once over the training set). **Done when:** output shape is predictable and correct for arbitrary input length (M7).
- **TTS-022: Mel viewer tool** — *(infra/audio; depends: 021)* `tools/mel` CLI rendering a log-mel spectrogram image. **Done when:** visually cross-checks against a reference implementation.
- **TTS-023: Pitch/energy inspection tools** — *(audio; depends: 017)* `tools/pitch` (autocorrelation-based F0 track) and `tools/energy` (short-time RMS curve) for later training-time diagnostics. **Done when:** both produce sensible curves on a known sustained-vowel recording.
- **TTS-024: ISTFT-based reconstruction experiment** — *(audio; depends: 015)* `WAV → STFT → ISTFT → WAV` round trip on real speech. **Done when:** reconstructed audio is audibly near-identical to the original.
- **TTS-025: Griffin-Lim** — *(audio; depends: 014, 015, 021)* Iterative phase estimation from a magnitude/mel spectrogram (30–60 iterations, alternating ISTFT/STFT). **Done when:** reconstructed audio from your own voice's mel spectrogram is intelligible, characteristically buzzy (M8).
- **TTS-026: Reconstruction-error metric + parameter sweep** — *(audio; depends: 025)* A numeric spectral-distance metric (e.g. mean absolute log-mel error) between original and Griffin-Lim-reconstructed audio; sweep `num_mels`, FFT size, hop size, iteration count. **Done when:** you have a logged table/plot of reconstruction error vs each parameter, before writing any neural vocoder code.

---

## Phase 4 — Tensor and Autograd Core (Track A, M9)

- **TTS-027: Tensor class** — *(framework; depends: 001)* Arbitrary-rank tensor with explicit strides, contiguous/row-major storage, shape introspection. **Done when:** construct/reshape/print works for rank 1–4 tensors.
- **TTS-028: Broadcasting** — *(framework; depends: 027)* Elementwise-op broadcasting rules (NumPy-style). **Done when:** unit tests cover scalar+tensor, vector+matrix, and mismatched-but-compatible shape cases.
- **TTS-029: Slicing, concat, transpose, reshape** — *(framework; depends: 027)* Core shape-manipulation ops needed for sequence data. **Done when:** each op has a shape-correctness test, including edge cases (empty slice, transpose of a 1D tensor).
- **TTS-030: Elementwise ops + gradients** — *(framework; depends: 027, 028)* add, sub, mul, div, exp, log, with hand-derived backward functions registered per op. **Done when:** each has a passing numerical-gradient check (see TTS-035).
- **TTS-031: Matmul + gradient** — *(framework; depends: 027)* Batched matrix multiply and its backward pass. **Done when:** gradient-check passes for random shapes including batched (3D) inputs.
- **TTS-032: Reductions + gradients** — *(framework; depends: 030)* sum/mean over an arbitrary axis, with correct gradient broadcasting back to the input shape. **Done when:** gradient-check passes.
- **TTS-033: Computation graph / Node** — *(framework; depends: 030)* Each op records a `Node` referencing its inputs and a backward closure; dynamic (define-by-run) graph construction. **Done when:** a graph for a small expression (e.g. `(a*b)+c`) can be inspected and its topology matches expectations.
- **TTS-034: Reverse-mode backward()** — *(framework; depends: 033)* Topological sort from the loss node, chain-rule gradient propagation, with gradient **accumulation** (not overwrite) when a tensor feeds multiple consumers. **Done when:** a shared-tensor-reuse test (same tensor used twice in a graph) produces the correctly summed gradient (M9, part 1).
- **TTS-035: Numerical gradient-checking utility** — *(framework; depends: 034)* Central finite-difference gradient checker (`ε≈1e-4`, relative-error threshold), reusable for any op. **Done when:** utility runs against TTS-030/031/032 and reports pass/fail automatically (M9, part 2).
- **TTS-036: Gradient-check regression suite** — *(framework; depends: 035)* Wire the checker into `ctest` so every future op addition is checked without manual invocation. **Done when:** adding a new op with no gradient-check test is a visible gap (e.g. a checklist or test-coverage note in the PR/commit).

---

## Phase 5 — NN Layers and Optimizers (Track A)

- **TTS-037: Linear layer** — *(framework; depends: 031, 034)* Weight + bias, forward + backward via autograd. **Done when:** gradient-check passes; overfits a toy linear-regression dataset.
- **TTS-038: Embedding layer (gather)** — *(framework; depends: 029, 034)* Integer-ID lookup into a learnable weight matrix, with correct backward (scatter-add into the right rows). **Done when:** gradient-check passes, including repeated-index scatter-add correctness.
- **TTS-039: Softmax / log-softmax** — *(framework; depends: 030, 032)* Numerically stable (max-subtraction) implementation. **Done when:** output rows sum to 1; gradient-check passes.
- **TTS-040: Masking utilities** — *(framework; depends: 028)* Apply `-inf` masks pre-softmax (for attention) and zero-masks post-computation (for loss), given a length tensor per batch element. **Done when:** a padded-batch test shows zero contribution from padded positions to both attention weights and loss.
- **TTS-041: LayerNorm** — *(framework; depends: 030, 032, 034)* Standard layer normalization with learnable scale/shift. **Done when:** gradient-check passes; forward output has ~zero mean/unit variance per normalized axis.
- **TTS-042: SGD optimizer** — *(framework; depends: 034)* Basic parameter update loop. **Done when:** converges on a toy least-squares problem.
- **TTS-043: Adam optimizer** — *(framework; depends: 034)* Moment estimates + bias correction. **Done when:** converges faster than SGD on the same toy problem.
- **TTS-044: Gradient clipping** — *(framework; depends: 034)* Clip by global norm. **Done when:** unit test confirms gradients are rescaled correctly when norm exceeds threshold, untouched otherwise.
- **TTS-045: LR scheduler** — *(framework; depends: 043)* Warmup + decay schedule. **Done when:** logged LR curve matches the configured schedule shape.
- **TTS-046: Checkpointing** — *(framework/infra; depends: 043)* Serialize/deserialize model parameters + optimizer state to disk. **Done when:** a training run can be killed and resumed from a checkpoint with identical loss trajectory.

---

## Phase 6 — Sequence Modeling (Track A, M10–M11)

- **TTS-047: RNN cell** — *(framework; depends: 037, 034)* Vanilla `tanh` recurrent cell, unrolled over a sequence. **Done when:** gradient-check passes across multiple timesteps (checks backprop-through-time).
- **TTS-048: LSTM (or GRU) cell** — *(framework; depends: 047)* Gated recurrent cell. **Done when:** gradient-check passes; empirically trains better than vanilla RNN on a longer toy sequence.
- **TTS-049: Toy sequence-copy experiment** — *(model; depends: 048, 043)* Train the recurrent cell to reproduce a random input sequence at the output. **Done when:** loss → ~0 (M10).
- **TTS-050: Toy sequence-reversal experiment** — *(model; depends: 049)* Encoder-decoder pair trained to reverse a sequence, forcing the encoder to summarize the *entire* input before any output. **Done when:** loss → ~0 on held-out random sequences.
- **TTS-051: Attention module** — *(framework; depends: 039, 040)* Dot-product (or additive) attention: scores against encoder states, softmax, weighted context vector. **Done when:** gradient-check passes; attention weights sum to 1 over unmasked positions.
- **TTS-052: Seq2seq encoder-decoder wrapper** — *(model; depends: 048, 051)* Generic encoder (RNN/LSTM) + attention + decoder (RNN/LSTM) wiring, reusable for both toy tasks and the real acoustic model. **Done when:** toy sequence-reversal task (TTS-050) trains through this wrapper, not a bespoke implementation.
- **TTS-053: Variable-length batching + masking toy experiment** — *(model; depends: 040, 052)* Train on padded, variable-length toy sequences; verify masking prevents any leakage from padded positions. **Done when:** loss curve is unaffected by padding amount (i.e. adding more padding to a batch doesn't change the loss on real positions).
- **TTS-054: Toy alignment task + attention visualization** — *(model; depends: 052)* Synthetic task where each input token maps to a variable-length span of outputs; visualize the resulting attention matrix. **Done when:** attention matrix shows a clean, near-diagonal, low-entropy pattern (M11).

---

## Phase 7 — Dataset and Text Frontend (Track C, M12–M13)

- **TTS-055: Recording script / phoneme-coverage tool** — *(text/infra; depends: none)* Given a phoneme inventory (e.g. ARPABET) and a candidate sentence pool, select/report a subset maximizing phoneme coverage (treat as set cover). **Done when:** tool outputs a sentence list with a coverage report (which phonemes appear, in how many contexts).
- **TTS-056: Recording validation script** — *(audio/infra; depends: 004)* Automated per-clip checks: clipping %, peak/RMS level, duration sanity, silence padding presence. **Done when:** flags a deliberately-clipped test file and passes a clean one.
- **TTS-057: Segmentation + transcript matching** — *(text/infra; depends: 056)* Split raw recording session(s) into per-utterance WAV files matched against transcript lines. **Done when:** produces one WAV + transcript pair per line with correct boundaries on a test recording session.
- **TTS-058: Resampling + normalization pipeline** — *(audio; depends: 004, 005)* Consistent downsample (e.g. 44.1k/48k → 22050 Hz) with proper anti-aliasing filter, plus peak/RMS normalization. **Done when:** output sample rate/level matches spec; downsampling without the filter is demonstrably worse (regression test comparing filtered vs naive).
- **TTS-059: Batch mel-generation pipeline** — *(audio/infra; depends: 021, 058)* Precompute and cache mel spectrograms to disk for the whole dataset, using fixed, versioned parameters. **Done when:** re-running with unchanged parameters produces byte-identical cached mels (cache-correctness check).
- **TTS-060: Dataset metadata index** — *(infra; depends: 057, 059)* Writer/reader for the `id | text | wav_path | duration | mel_path | token_path` format from Section 14. **Done when:** index round-trips and can be loaded into memory for batching.
- **TTS-061: Train/val/test splitter** — *(infra; depends: 060)* Utterance-level split with no duplicate/near-duplicate leakage across splits. **Done when:** automated leakage check finds zero overlapping utterance IDs across splits.
- **TTS-062: Text normalization module** — *(text; depends: none)* Number expansion, abbreviation handling, casing, symbol handling. **Done when:** unit tests cover numbers, common abbreviations, and punctuation edge cases.
- **TTS-063: Character tokenizer + vocabulary** — *(text; depends: 062)* Char-level tokenizer with a fixed vocabulary (for the earliest toy text-to-mel experiments). **Done when:** round-trips text → IDs → text exactly for in-vocabulary input.
- **TTS-064: G2P / phonemizer integration** — *(text; depends: 062)* Wire in an external G2P library/dictionary (acceptable per Section 32) with a fallback for out-of-vocabulary words. **Done when:** phonemizes a sample sentence and the output matches expected ARPABET/IPA phonemes for known words.
- **TTS-065: Text→ID pipeline** — *(text; depends: 063 or 064)* Full `text → normalize → tokenize/phonemize → integer IDs` pipeline used by the dataset loader. **Done when:** vocabulary coverage check against your recorded corpus shows no unexpected OOV rate (M13).
- **TTS-066: Dataset loader with batching/padding/masks** — *(infra; depends: 060, 065, 040)* Produces batches of `(padded phoneme IDs, padded mel targets, length masks)` from the dataset index. **Done when:** a batch loads with correct shapes and mask values verified against known utterance lengths (M12).

---

## Phase 8 — Acoustic Model: Phonemes → Mel (Track A+B+C convergence, M14–M15)

- **TTS-067: Phoneme/char embedding + bidirectional encoder** — *(model; depends: 038, 048)* Embedding lookup into a bidirectional RNN/LSTM encoder producing `[batch, N, hidden_dim]` states. **Done when:** shape-correctness test on a padded batch, encoder output finite/no NaNs.
- **TTS-068: Decoder pre-net** — *(model; depends: 037)* Small feed-forward bottleneck applied to the previous mel frame before it enters the decoder step. **Done when:** gradient-check passes as part of the full decoder step.
- **TTS-069: Decoder step wired to attention** — *(model; depends: 051, 068)* One autoregressive decoder step: attention over encoder states → context vector → LSTM/RNN update → mel projection. **Done when:** a single forward step produces the expected `[batch, n_mels]` output shape.
- **TTS-070: Stop-token head** — *(model; depends: 069)* Linear + sigmoid predicting continue/stop per frame. **Done when:** BCE loss computes correctly against a ground-truth stop-frame label.
- **TTS-071: Teacher-forcing training loop** — *(model; depends: 069, 070, 066)* Full unrolled decoder loop over a batch using ground-truth previous mel frames as input. **Done when:** runs end-to-end on a batch without shape errors or NaNs.
- **TTS-072: Mel + stop-token loss** — *(model; depends: 071)* L1 mel-reconstruction loss + BCE stop-token loss, masked to ignore padded frames. **Done when:** loss value matches a hand-computed value on a tiny synthetic batch.
- **TTS-073: Overfit-tiny-dataset harness** — *(model/infra; depends: 072, 044, 045, 046)* Training script targeting 1–10 utterances, expected to drive loss to near-zero. **Done when:** loss curve flattens near zero and generated mel visually matches the target, with a clean attention matrix (M14).
- **TTS-074: Full training loop + monitoring** — *(model/infra; depends: 073)* Batching across the real dataset, validation pass, checkpointing, and logging of: train/val loss, mel error, attention alignment snapshot, gradient norms, throughput. **Done when:** a multi-epoch run produces a logged history file and periodic sample-generation outputs (audio + attention plot).
- **TTS-075: Non-teacher-forced (free-running) inference** — *(model; depends: 074)* Autoregressive generation using the model's own previous prediction instead of ground truth, with stop-token-driven termination. **Done when:** produces a variable-length mel spectrogram for an unseen sentence without crashing on runaway generation (add a max-length safety cap).
- **TTS-076: Train on meaningful-scale dataset** — *(model; depends: 074, 075)* Scale training up to the "meaningful experiment" dataset size (Section 12). **Done when:** validation loss is reasonable and attention stays clean on held-out sentences of similar length (M15).

---

## Phase 9 — Vocoder (Track B extension, M16–M17)

- **TTS-077: Griffin-Lim on model-generated mels** — *(audio/model; depends: 025, 075)* Feed the acoustic model's *predicted* mel (not ground truth) through the existing Griffin-Lim implementation. **Done when:** full text→mel→Griffin-Lim→WAV pipeline produces buzzy but intelligible speech (M16).
- **TTS-078: Dilated causal convolution layer** — *(framework/model; depends: 029, 034)* 1D convolution with dilation and causal padding (no future leakage), plus backward pass. **Done when:** gradient-check passes; causality verified by a unit test (output at time t unaffected by changing input at t+1).
- **TTS-079: Mel-conditioning / upsampling** — *(model; depends: 078)* Upsample mel-frame-rate conditioning to the audio sample rate (e.g. transposed conv or repeat+interpolate) to feed each convolution layer. **Done when:** output length matches expected upsampled length exactly.
- **TTS-080: Autoregressive vocoder architecture** — *(model; depends: 078, 079)* Stack of dilated causal conv blocks (WaveNet-style) predicting one audio sample at a time, conditioned on local mel context. **Done when:** forward pass produces a `[batch, num_samples]` output with correct shape.
- **TTS-081: Vocoder loss + training loop** — *(model; depends: 080, 043)* Training on `(mel, audio)` pairs, appropriate reconstruction/classification loss for the sample-generation formulation chosen (e.g. categorical cross-entropy over quantized samples, or regression). **Done when:** loss decreases monotonically on a small batch.
- **TTS-082: Vocoder overfit-tiny-dataset test** — *(model/infra; depends: 081)* Same discipline as TTS-073, applied to the vocoder alone. **Done when:** vocoder reconstructs a handful of memorized training clips with clearly higher fidelity than Griffin-Lim on the same mels.
- **TTS-083: Vocoder full-dataset training** — *(model; depends: 082)* Train on the full dataset's `(mel, audio)` pairs. **Done when:** generated audio from held-out mels is clearly higher quality than Griffin-Lim on the same inputs, evaluated by ear and by a spectral-distance metric (M17).

---

## Phase 10 — Integration, CUDA, Complete System (M18)

- **TTS-084: End-to-end inference pipeline** — *(infra/model; depends: 076, 083)* Wire `text → normalize → phonemize → IDs → acoustic model → predicted mel → vocoder → WAV` into one callable pipeline. **Done when:** a single function call takes a raw sentence string and returns a playable WAV buffer.
- **TTS-085: `synthesize` CLI tool** — *(infra; depends: 084)* `tools/synthesize "some sentence"` writing a `.wav` file. **Done when:** produces audible, recognizable speech in your own voice for a novel sentence (M18).
- **TTS-086: Profiling harness** — *(infra; depends: 074, 081)* Wall-clock/step-time profiling of the training loops to find real bottlenecks before writing any CUDA. **Done when:** a profiling report ranks time spent per subsystem (matmul, recurrent step, conv, data loading).
- **TTS-087: CUDA matmul kernel** — *(framework/cuda; depends: 031, 086)* Custom GPU kernel for the highest-impact bottleneck identified in TTS-086 (expected: matmul). **Done when:** numerically matches the CPU implementation within tolerance and is measurably faster at realistic batch sizes.
- **TTS-088: CUDA attention/softmax kernel** — *(framework/cuda; depends: 051, 087)* GPU-accelerated batched attention score computation + softmax. **Done when:** matches CPU output; end-to-end training step time improves.
- **TTS-089: CUDA dilated-conv kernel** — *(framework/cuda; depends: 078, 087)* GPU kernel for the vocoder's dilated causal convolutions, if profiling (TTS-086, re-run after 087/088) shows it's still a bottleneck. **Done when:** matches CPU output; vocoder training throughput improves measurably.
- **TTS-090: GPU training re-validation** — *(infra; depends: 087, 088, 089)* Re-run the overfit-tiny-dataset tests (TTS-073, TTS-082) on GPU to confirm CUDA kernels didn't silently change numerical behavior. **Done when:** GPU-trained overfit runs match CPU-trained loss curves within expected floating-point tolerance.
- **TTS-091: M18 final acceptance pass** — *(infra; depends: 085, 090)* Full read-through of Section 27's failure-mode checklist against the finished system; fix or explicitly accept any known rough edges. **Done when:** you can type an arbitrary sentence and hear it spoken back in your own voice, end to end, with no manual intervention (M18 complete).

---

## Notes on sequencing

- Phases 1–3 (Track B / audio) and Phases 4–6 (Track A / framework) have no code dependency on each other and can be worked in parallel or interleaved — see `DESIGN.md` Section 2.
- Phase 7 (Track C / text + dataset) is lightweight and can start any time before Phase 8; don't front-load it.
- Do not start Phase 8 before Phase 6's toy alignment task (TTS-054) produces a clean attention matrix — a broken toy attention implementation will make a broken real acoustic model indistinguishable from a data problem.
- Do not start Phase 9 (vocoder) before Phase 3's Griffin-Lim baseline (TTS-025/026) is done — you need a signal-processing baseline to know whether the neural vocoder is actually adding value.
- Within Phase 10, CUDA tickets (087–090) are explicitly gated on profiling (086) — don't reorder them ahead of it "just in case."
