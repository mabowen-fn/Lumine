#include "lumine/convolver.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

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

    Image Convolver::convolve_horizontal(const Image& input, const std::vector<float>& kx, Padding pad, int stride_x) {
        const int kw = (int)kx.size();
        const int pad_x = kw/2;
        const int out_w = div_up(input.width() + 2*pad_x - kw + 1, std::max(1, stride_x));
        Image out(out_w, input.height(), input.channels());

        for(int c = 0; c<input.channels(); ++c) {
            for(int y = 0; y < input.height(); ++y) {
                for (int ox=0, ix=-pad_x; ox<out_w; ++ox, ix+=stride_x) {
                    float acc = 0.0f;
                    for (int kx_i=0; kx_i<kw; ++kx_i) {
                        int sx = ix + kx_i;
                        float v = sample(input, sx, y, c, pad);
                        acc += v * kx[kx_i];
                    }
                    out.at(ox, y, c) = acc;
                }
            }
            return out;
        }
    }

    Image Convolver::convolve_vertical(const Image& input, const std::vector<float>& ky, Padding pad, int stride_y) {
        const int kh = (int)ky.size();
        const int pad_y = kh/2;
        const int out_h = div_up(input.height() + 2*pad_y - kh + 1, std::max(1, stride_y));
        Image out(input.width(), out_h, input.channels());

        for(int c=0; c<input.channels(); ++c) {
            for(int x = 0; x < input.width(); ++x) {
                for(int oy = 0, iy=-pad_y; oy<out_h; ++oy, iy+=stride_y) {
                    float acc = 0.0f;
                    for (int ky_i=0; ky_i<kh;++ky_i) {
                        int sy = iy + ky_i;
                        float v = sample(input, x, sy, c, pad);
                        acc += v * ky[ky_i];
                    }
                    out.at(x, oy, c) = acc;
                }
            }
        }
        return out;
    }

    Image Convolver::convolve(const Image& input, const Kernel& K, const ConvParams& params){
// Try separable fast path
std::vector<float> ky, kx;
if(K.try_separable(ky, kx)){
// Horizontal (stride on X), then vertical (stride on Y)
Image tmp = convolve_horizontal(input, kx, params.padding, params.stride);
Image out = convolve_vertical(tmp, ky, params.padding, params.stride);


// Visualization post-processing
float global_min = std::numeric_limits<float>::infinity();
float global_max = -std::numeric_limits<float>::infinity();
for(int c=0;c<out.channels();++c)
for(int y=0;y<out.height();++y)
for(int x=0;x<out.width();++x){
float v = out.at(x,y,c);
global_min = std::min(global_min, v);
global_max = std::max(global_max, v);
}


for(int c=0;c<out.channels();++c)
for(int y=0;y<out.height();++y)
for(int x=0;x<out.width();++x){
float v = out.at(x,y,c);
switch(params.viz){
case VizMode::Clamp: v = std::clamp(v, 0.0f, 1.0f); break;
case VizMode::Normalize:
if(global_max!=global_min) v = (v-global_min)/(global_max-global_min); else v=0.5f; break;
case VizMode::None: break;
}
out.at(x,y,c)=v;
}
return out;
}


// Fallback: full 2D convolution
const int kw = K.width();
const int kh = K.height();
const int pad_x = kw/2; // symmetric
const int pad_y = kh/2;
const int stride = std::max(1, params.stride);


const int out_w = div_up(input.width() + 2*pad_x - kw + 1, stride);
const int out_h = div_up(input.height() + 2*pad_y - kh + 1, stride);
Image out(out_w, out_h, input.channels());


float global_min = std::numeric_limits<float>::infinity();
float global_max = -std::numeric_limits<float>::infinity();


for(int c=0;c<input.channels();++c){
for(int oy=0, iy=-pad_y; oy<out_h; ++oy, iy+=stride){
for(int ox=0, ix=-pad_x; ox<out_w; ++ox, ix+=stride){
float acc = 0.0f;
for(int ky_i=0; ky_i<kh; ++ky_i){
for(int kx_i=0; kx_i<kw; ++kx_i){
int sx = ix + kx_i;
int sy = iy + ky_i;
float v = sample(input, sx, sy, c, params.padding);
acc += v * K.weights()[(size_t)ky_i*kw + kx_i];
}
}
out.at(ox,oy,c) = acc;
global_min = std::min(global_min, acc);
global_max = std::max(global_max, acc);
}
}
}


}
}