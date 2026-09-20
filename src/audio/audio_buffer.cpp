#include "audio_buffer.h"
#include <iostream>
// Data Layout
//  uint32_t sample_rate;
//  uint16_t bit_depth;
//  uint16_t channels;
//
//  std::vector<float> samples;
namespace Audio {

AudioBuffer load_PCM16(const WavFile &wav) {
  if (wav.format.audio_format != 1) {
    throw std::runtime_error(
        "WavFile has no valid sample_rate format to load to the audio buffer");
  }
  if (wav.format.bits_per_sample != 16) {
    throw std::runtime_error(
        "WavFile bit per sample must be 16 bit PCM to the audio buffer");
  }

  AudioBuffer buffer{};

  buffer.sample_rate = wav.format.sample_rate;
  buffer.channels = wav.format.num_channels;

  buffer.samples.reserve(wav.data.size());
  for (size_t i = 0; i < wav.data.size(); ++i) {
    float x = static_cast<float>(wav.data[i]) / 32768.0f;
    buffer.samples.push_back(x);
  }

  return buffer;
}

void debug(const AudioBuffer &buffer) {

  if (buffer.samples.empty()) {
    throw std::runtime_error("|DEBUG: sample buffer is empty\n ");
  }
  std::cout << "\n========== AudioBuffer DEBUG ==========\n";

  std::cout << "Format\n";
  std::cout << "  Channels        : " << buffer.channels << "\n";
  std::cout << "  Sample rate     : " << buffer.sample_rate << " Hz\n";

  std::cout << "\nData\n";
  std::cout << "  Data bytes      : " << buffer.samples.size() << "\n";

  std::cout << "\nRaw bytes preview\n";
  constexpr std::size_t preview_count = 32;

  const std::size_t count = std::min(preview_count, buffer.samples.size());

  std::cout << "  ";

  for (std::size_t i = 0; i < count; ++i) {
    std::cout << buffer.samples[i];

    if (i + 1 < count) {
      std::cout << ", ";
    }
  }

  if (buffer.samples.size() > preview_count) {
    std::cout << ", ...";
  }

  std::cout << "\n";

  std::cout << "================================\n";
}
}; // namespace Audio
