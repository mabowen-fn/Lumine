#pragma once
#include <vector>
#include <string>

namespace lumine {
class Kernel {
    public:
        Kernel() = default;
        Kernel(int w, int h, std::vector<float> weights);

        static Kernel from_builtin(const std::string& name); // filter name
        static Kernel from_string(const std::string& spec); // custom filter

        int width() const { return m_w; }
        int height() const { return m_h; }
        const std::vector<float>& weights() const { return m_wts; }

    private:
        int m_w{0}, m_h{0};
        std::vector<float> m_wts; // row-major order h x w
};
}