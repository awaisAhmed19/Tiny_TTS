#pragma once
#include <stdint.h>
#include <string>
#include <vector>
struct WavFormat {
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
};

struct WavFile {
  WavFormat format;
  std::vector<int16_t> data;
};
namespace Wav {
WavFile read_wav(const std::string &file_path);
void debug(const WavFile &wav);
void write_wav(const WavFile &wav);
}; // namespace Wav
