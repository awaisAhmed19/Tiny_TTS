#include "audio/audio_buffer.h"
#include "audio/wav_file.h"
using namespace std;

int main() {
  const std::string file_path =
      "/home/awais/DEV/Tiny_TTS/src/dataset/sample_audios/sample-3s.wav";
  WavFile wav;
  wav = Wav::read_wav(file_path);
  Wav::debug(wav);

  Audio::AudioBuffer buffer;
  buffer = Audio::load_PCM16(wav);
  Audio::debug(buffer);
  Wav::write_wav(wav);
  return 0;
}
