#include "lumine/convolver.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace lumine {

    static int div_up(int a, int b) { return (a + b - 1) / b; }

    float Convolver::sample(const Image& img, int x, int y, int c, Padding pad) {
        if (x >= 0 && x < img.width() && y >= 0 && y < img.height())
            return img.at(x, y, c);
        if (pad == Padding::ZERO)
            return 0.0f;
        x = std::min(std::max(x, 0), img.width() - 1);
        y = std::min(std::max(y, 0), img.height() - 1);
        return img.at(x, y, c);
    }

    Image Convolver::convolve(const Image& input, const Kernel& K, const ConvParams& params) {
        const int kw = K.width();
        const int kh = K.height();
        // [TODO]: symetric padding, termporaryly only support odd-sized kernels
        const int pad_x = kw / 2;
        const int pad_y = kh / 2;
        const int stride = std::max(1, params.stride);
        
        const int out_w = div_up(input.width() + 2 * pad_x - kw + 1, stride);
        const int out_h = div_up(input.height() + 2 * pad_y - kh + 1, stride);
        Image out(out_w, out_h, input.channels());

        for (int c = 0; c < input.channels(); ++c) {
            for (int oy = 0, iy = -pad_y; oy < out_h; ++oy, iy +=stride) {
                for (int ox = 0, ix =- pad_x; ox < out_w; ++ox, ix+=stride) {
                    float sum = 0.0f;
                    for (int ky = 0; ky < kh; ++ky) {
                        for (int kx = 0; kx < kw; ++kx) {
                            int sx = ix + kx;
                            int sy = iy + ky;
                            float v = sample(input, sx, sy, c, params.padding);
                            sum += v * K.weights()[(size_t)ky*kw + kx];
                        }
                    }
                    out.at(ox, oy, c) = sum;
                }
            }
        }

        for (int c = 0; c < out.channels(); ++c) {
            for (int y = 0; y < out.height(); ++y) {
                for (int x = 0; x < out.width(); ++x) {
                    out.at(x, y, c) = std::clamp(out.at(x, y, c), 0.0f, 1.0f);
                }
            }
        }

        return out;        

    }
}