#pragma once
#include <math/ml/device.h>
#include <math/ml/types.h>

#include <cstddef>

struct Buffer {

  void *data;
  size_t bytes;
  Device device;
  uint32_t ref_count;
};
