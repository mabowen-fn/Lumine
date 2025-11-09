#include "lumine/kernel.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

namespace lumine {

Kernel::Kernel(int w, int h, std::vector<float> weights)
: m_w(w), m_h(h), m_wts(std::move(weights)) {
    if ((int)m_wts.size() != w * h)
        throw std::runtime_error("Kernel weights size does not match dimensions");
}

static Kernel make_box(int k) {
    int w=k,h=k; std::vector<float> wt((size_t)w*h, 1.0f/float(w*h));
    return Kernel(w,h,std::move(wt));
}


static Kernel make_identity() {
    return Kernel(3,3,{0,0,0,0,1,0,0,0,0});
}


static Kernel make_sharpen() {
    return Kernel(3,3,{0,-1,0,-1,5,-1,0,-1,0});
}


static Kernel make_sobel_x() {
    return Kernel(3,3,{-1,0,1,-2,0,2,-1,0,1});
}


static Kernel make_sobel_y() {
    return Kernel(3,3,{-1,-2,-1,0,0,0,1,2,1});
}


static Kernel make_gauss5() {
    // 5x5 separable approx (sigma~1), normalized
    std::vector<float> row = {1, 4, 6, 4, 1};
    float sum = 0.f; for(float v: row) sum += v; for(float& v: row) v/=sum;
    std::vector<float> w(25);
    for(int y=0;y<5;++y) for(int x=0;x<5;++x) w[y*5+x] = row[y]*row[x];
    return Kernel(5,5,std::move(w));
}


Kernel Kernel::from_builtin(const std::string& name) {
    std::string n=name; std::transform(n.begin(), n.end(), n.begin(), [](unsigned char c){ return std::tolower(c); });
    if(n=="identity") return make_identity();
    if(n=="box3") return make_box(3);
    if(n=="box5") return make_box(5);
    if(n=="sharpen") return make_sharpen();
    if(n=="sobel_x") return make_sobel_x();
    if(n=="sobel_y") return make_sobel_y();
    if(n=="gauss5") return make_gauss5();
    throw std::runtime_error("Unknown builtin kernel: " + name);
}


Kernel Kernel::from_string(const std::string& spec) {
    // Format: rows separated by ';', columns by whitespace or comma
    std::vector<std::vector<float>> rows;
    std::stringstream ss(spec);
    std::string row;
    while(std::getline(ss, row, ';')){
        std::stringstream rs(row);
        std::string tok; std::vector<float> vals;
        while(rs >> tok){
        // allow commas
            if(!tok.empty() && tok.back()==',') tok.pop_back();
                vals.push_back(std::stof(tok));
        }
        if(!vals.empty()) rows.push_back(std::move(vals));
    }
    if(rows.empty()) throw std::runtime_error("Empty kernel spec");
    int h = (int)rows.size();
    int w = (int)rows[0].size();
    for(const auto& r : rows) if((int)r.size()!=w) throw std::runtime_error("Inconsistent kernel row widths");
        std::vector<float> wt; wt.reserve((size_t)w*h);
    for(const auto& r: rows) wt.insert(wt.end(), r.begin(), r.end());
    return Kernel(w,h,std::move(wt));
}

bool Kernel::try_separable(std::vector<float>& ky, std::vector<float>& kx, float eps) const {
    if (m_w==0 || m_h==0) return false;

    int py=-1, px=-1;
    for(int i = 0; i < m_h && py ==-1; ++i) {
        for(int j = 0; j< m_w; ++j) {
            if(std::fabs(m_wts[(size_t)i*m_w + j]) > eps) { py=i; px=j; break; }
        }
    }
    if(py==-1) {
        ky.assign(m_h, 1.0f);
        kx.assign(m_w, 0.0f);
        return true;
    }

    kx.assign(m_w, 0.0f);
    ky.assign(m_h, 0.0f);

    for (int x = 0; x < m_w; ++x) kx[x] = m_wts[(size_t)py*m_w + x];
    float pivot = m_wts[(size_t)py*m_w + px];
    for (int y = 0; y < m_h; ++y) ky[y] = m_wts[(size_t)y*m_w + px] / pivot;

    for (int i=0; i < m_h; ++i) {
        for(int j=0; j < m_w; ++j) {
            float a = m_wts[(size_t)i*m_w + j];
            float b = ky[i]*kx[j];
            if(std::fabs(a - b) > eps * (1.0f + std::fabs(a))) return false;
        }
    }

    return true;
}
}