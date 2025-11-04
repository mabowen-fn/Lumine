#pragma once
#include "image.hpp"
#include "kernel.hpp"
#include "types.hpp"

namespace lumine {

struct ConvParams {
    int stride{1};
    Padding padding{Padding::ZERO};
};

class Convolver {
    public:
        static Image convolve(const Image& input, const Kernel& kernel, const ConvParams& params);
    private:
        static float sample(const Image& img, int x, int y, int c, Padding pad);
};
}