#include "wav_reader.h"
#include <algorithm>
#include <fstream>
#include <iostream>
namespace Wav {

WavFile read_wav(const std::string &file_path) {
  std::ifstream file(file_path, std::ios::binary);
  if (!file) {
    throw std::runtime_error("couldnt open the file");
  }

  char riff_id[4];
  uint32_t chunk_size;
  char wave_id[4];

  file.read(riff_id, 4);
  file.read(reinterpret_cast<char *>(&chunk_size), 4);
  file.read(wave_id, 4);

  if (std::string_view(riff_id, 4) != "RIFF") {
    throw std::runtime_error("Not a RIFF file");
  }

  if (std::string_view(wave_id, 4) != "WAVE") {
    throw std::runtime_error("Not a WAVE file");
  }
  WavFile wav{};
  bool found_fmt = false;
  bool found_data = false;

  while (file && (!found_fmt || !found_data)) {
    char chunk_id[4];
    uint32_t subchunk_size;

    file.read(chunk_id, 4);
    file.read(reinterpret_cast<char *>(&subchunk_size), 4);

    if (!file) {
      break;
    }

    std::string_view id(chunk_id, 4);

    if (id == "fmt ") {
      if (subchunk_size < 16) {
        throw std::runtime_error("Invalid fmt chunk");
      }

      file.read(reinterpret_cast<char *>(&wav.format.audio_format),
                sizeof wav.format.audio_format);
      file.read(reinterpret_cast<char *>(&wav.format.num_channels),
                sizeof wav.format.num_channels);
      file.read(reinterpret_cast<char *>(&wav.format.sample_rate),
                sizeof wav.format.sample_rate);
      file.read(reinterpret_cast<char *>(&wav.format.byte_rate),
                sizeof wav.format.byte_rate);
      file.read(reinterpret_cast<char *>(&wav.format.block_align),
                sizeof wav.format.block_align);
      file.read(reinterpret_cast<char *>(&wav.format.bits_per_sample),
                sizeof wav.format.bits_per_sample);

      if (subchunk_size > 16) {
        file.seekg(static_cast<std::streamoff>(subchunk_size - 16),
                   std::ios::cur);
      }

      found_fmt = true;
    }

    else if (id == "data") {
      wav.data.resize(subchunk_size);

      file.read(reinterpret_cast<char *>(wav.data.data()), subchunk_size);

      if (!file) {
        throw std::runtime_error("Could not read WAV data");
      }
      found_data = true;
    } else {
      file.seekg(static_cast<std::streamoff>(subchunk_size), std::ios::cur);
    }

    if (subchunk_size % 2 != 0) {
      file.seekg(1, std::ios::cur);
    }
  }

  if (!found_fmt) {
    throw std::runtime_error("WAV file has no fmt chunk");
  }
  if (!found_data) {
    throw std::runtime_error("WAV file has no data chunk");
  }
  if (wav.format.audio_format != 1) {
    throw std::runtime_error("Only PCM WAV files are supported");
  }

  return wav;
}

void debug(const WavFile &wav) {
  const auto &fmt = wav.format;

  std::cout << "\n========== WAV DEBUG ==========\n";

  std::cout << "Format\n";
  std::cout << "  Audio format    : " << fmt.audio_format << "\n";
  std::cout << "  Channels        : " << fmt.num_channels << "\n";
  std::cout << "  Sample rate     : " << fmt.sample_rate << " Hz\n";
  std::cout << "  Bits/sample     : " << fmt.bits_per_sample << "\n";
  std::cout << "  Block align     : " << fmt.block_align << " bytes\n";
  std::cout << "  Byte rate       : " << fmt.byte_rate << " bytes/sec\n";

  std::cout << "\nData\n";
  std::cout << "  Data bytes      : " << wav.data.size() << "\n";

  if (fmt.block_align != 0) {
    const std::size_t frame_count = wav.data.size() / fmt.block_align;

    std::cout << "  Audio frames    : " << frame_count << "\n";

    if (fmt.sample_rate != 0) {
      double duration = static_cast<double>(frame_count) / fmt.sample_rate;

      std::cout << "  Duration        : " << duration << " sec\n";
    }
  }

  std::cout << "\nRaw bytes preview\n";
  constexpr std::size_t preview_count = 32;

  const std::size_t count = std::min(preview_count, wav.data.size());

  std::cout << "  ";

  for (std::size_t i = 0; i < count; ++i) {
    std::cout << static_cast<int>(wav.data[i]);

    if (i + 1 < count) {
      std::cout << ", ";
    }
  }

  if (wav.data.size() > preview_count) {
    std::cout << ", ...";
  }

  std::cout << "\n";

  std::cout << "================================\n";
}

}; // namespace Wav
