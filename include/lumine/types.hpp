#pragma once
#include <cstdlib>
#include <string>

namespace lumine {
    enum class Padding {
        ZERO,
        EDGE,
    };
    enum class VizMode {
        Clamp,
        Normalize,
        None
    };
    struct Size { int width{0}; int height{0}; };
    // 8 bit RGB pixel
    struct RGB8 {size_t r, g, b;};

}