#include "audio/wav_reader.h"
using namespace std;

int main() {
  const std::string file_path =
      "/home/awais/DEV/Tiny_TTS/src/dataset/sample_audios/sample-6s.wav";
  WavFile wav;
  wav = Wav::read_wav(file_path);
  Wav::debug(wav);
  return 0;
}
