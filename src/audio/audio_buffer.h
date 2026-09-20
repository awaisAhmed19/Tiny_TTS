#include "wav_file.h"
#include <stdint.h>
#include <vector>
namespace Audio {

struct AudioBuffer {
  uint32_t sample_rate;
  uint16_t channels;

  std::vector<float> samples;
};

AudioBuffer load_PCM16(const WavFile &wav);
void debug(const AudioBuffer &buffer);
}; // namespace Audio
