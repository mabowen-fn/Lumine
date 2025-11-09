#pragma once
#include "image.hpp"
#include "kernel.hpp"
#include "types.hpp"

namespace lumine {

struct ConvParams {
    int stride{1};
    Padding padding{Padding::ZERO};
    VizMode viz{VizMode::Clamp};
};

class Convolver {
    public:
        static Image convolve(const Image& input, const Kernel& kernel, const ConvParams& params);
    private:
        static float sample(const Image& img, int x, int y, int c, Padding pad);

        static Image convolve_horizontal(const Image& input, const std::vector<float>& kx, Padding pad, int stride_x);
        static Image convolve_vertical(const Image& input, const std::vector<float>& ky, Padding pad, int stride_y);
};
}