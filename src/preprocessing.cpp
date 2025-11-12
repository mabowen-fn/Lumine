#include "lumine/preprocessing.hpp" 
#include <algorithm>
#include <cmath>
#include <cassert>

namespace lumine {

Image Preprocessing::grayscale(const Image& input) {
  int width = input.width();
  int height = input.height();
  Image output(width, height, 1);

  for(int y = 0; y < height; ++y) {
    for(int x = 0; x < width; ++x) {
      float r = input.at(x, y, 0);
      float g = input.at(x, y, 1);
      float b = input.at(x, y, 2);

      float gray = 0.299f * r + 0.587f * g + 0.114f * b;
      output.at(x, y, 0) = gray;
    }
  }

  output.save("gray.jpg");
  return output;
}

Image Preprocessing::denoise(const Image& input) {
  int width = input.width();
  int height = input.height();
  Image output(width, height, input.channels());

  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      for (int c = 0; c < input.channels(); ++c) {
        std::vector<float> window;
        for (int ky = -1; ky <= 1; ++ky) {
          for (int kx = -1; kx <= 1; ++kx) {
            window.push_back(input.at(x + kx, y + ky, c));
            }
          }
          std::sort(window.begin(), window.end());
          output.at(x, y, c) = window[window.size() / 2];
      }
    }
  }
  output.save("denoise.jpg");
  return output;
}

Image Preprocessing::sauvola_binarization(const Image& input, float k, int window_size) {
  int width = input.width();
  int height = input.height();
  int half_window = window_size / 2;

  Image output(width, height, 1);

  for (int y = half_window; y < height - half_window; ++y) {
    for (int x = half_window; x < width - half_window; ++x) {
      float sum = 0.0f, sq_sum = 0.0f;
      for (int ky = -half_window; ky <= half_window; ++ky) {
        for (int kx = -half_window; kx <= half_window; ++kx) {
          float pixel = input.at(x + kx, y + ky, 0);
          sum += pixel;
          sq_sum += pixel * pixel;
        }
      }
      float mean = sum / (window_size * window_size);
      float stddev = std::sqrt(sq_sum / (window_size * window_size) - mean * mean);
      float threshold = mean * (1 + k * (stddev / 128 - 1));
      output.at(x, y, 0) = input.at(x, y, 0) > threshold ? 1.0f : 0.0f;
    }
  }

  output.save("sauvola_binarization.jpg");
  return output;
}

}
