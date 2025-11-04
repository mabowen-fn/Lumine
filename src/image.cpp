#include "lumine/image.hpp"
#include <stdexcept>
#include <cstring>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace lumine {

Image::Image(int w, int h, int c): m_width(w), m_height(h), m_channels(c), m_data((size_t)w*h*c, 0.0f) {}

Image Image::load(const std::string& path, bool force_grayscale) {
    int w, h, comp;
    stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &comp, force_grayscale ? 1 : 0);
    if (!pixels) throw std::runtime_error("Failed to load image: " + path);

    int ch = force_grayscale ? 1 : comp;
    if (ch != 1 && ch != 3 && ch != 4) {
        stbi_image_free(pixels);
        throw std::runtime_error("Unsupported number of channels: " + std::to_string(ch));
    }

    Image img(w, h, ch==4?3:ch);
    if (ch == 1) {
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                img.at(j, i, 0) = pixels[i*w + j] / 255.0f;
    }
    else {
        int stride = ch;
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j) {
                const stbi_uc* p = pixels + (i*w + j)*stride;
                img.at(j,i,0) = p[0] / 255.0f;
                img.at(j,i,1) = p[1] / 255.0f;
                img.at(j,i,2) = p[2] / 255.0f;
            }
    }
        stbi_image_free(pixels);
        return img;
}

void Image::save(const std::string& path) const {
    auto ends_with = [](const std::string& s, const std::string& suf){
        if(s.size()<suf.size()) return false; return std::equal(suf.rbegin(), suf.rend(), s.rbegin()); };


    // Convert planar float [0,1] -> interleaved 8-bit RGB/Gray
    std::vector<uint8_t> inter((size_t)m_width*m_height*(m_channels==1?1:3));
    if(m_channels==1){
        for(int y=0;y<m_height;++y) for(int x=0;x<m_width;++x){
            float v = std::clamp(at(x,y,0), 0.0f, 1.0f);
            inter[(size_t)y*m_width + x] = (uint8_t)(v*255.0f + 0.5f);
        }
    } else {
        for(int y=0;y<m_height;++y) for(int x=0;x<m_width;++x){
            for(int c=0;c<3;++c){
                float v = std::clamp(at(x,y,c), 0.0f, 1.0f);
                inter[((size_t)y*m_width + x)*3 + c] = (uint8_t)(v*255.0f + 0.5f);
            }
        }
}


if(ends_with(path, ".png")){
if(m_channels==1)
stbi_write_png(path.c_str(), m_width, m_height, 1, inter.data(), m_width);
else
stbi_write_png(path.c_str(), m_width, m_height, 3, inter.data(), m_width*3);
} else if(ends_with(path, ".jpg") || ends_with(path, ".jpeg")){
stbi_write_jpg(path.c_str(), m_width, m_height, (m_channels==1?1:3), inter.data(), 90);
} else if(ends_with(path, ".bmp")){
stbi_write_bmp(path.c_str(), m_width, m_height, (m_channels==1?1:3), inter.data());
} else {
throw std::runtime_error("Unsupported image format for save: " + path);
}
}

}