#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "types.hpp"

namespace lumine {

class Image {
    public:
        Image() = default;
        Image(int w, int h, int c = 1);

        static Image load(const std::string& path, bool force_grayscale = false);
        void save(const std::string& path) const;

        int width() const { return m_width; }
        int height() const { return m_height; }
        int channels() const { return m_channels; }

        float* data() { return m_data.data(); }
        const float* data() const { return m_data.data(); }

        float& at(int x, int y, int c = 0) { return m_data[(size_t)c*m_width*m_height + (size_t)y*m_width + x]; }
        const float& at(int x, int y, int c = 0) const { return m_data[(size_t)c*m_width*m_height + (size_t)y*m_width + x]; }

    private:
        int m_width{0}, m_height{0}, m_channels{1};
        std::vector<float> m_data;
};

}