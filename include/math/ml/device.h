#pragma once

enum class Devicetypes { CPU, CUDA };
struct Device {
  Devicetypes type;
  int id;
};
