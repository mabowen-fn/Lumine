#pragma once
#include "image.hpp"

namespace lumine {
class Preprocessing {
public:
  static Image grayscale(const Image& input);
  static Image denoise(const Image& input);
  static Image sauvola_binarization(const Image& input, float k = 0.2f, int window_size=15);
};
}
