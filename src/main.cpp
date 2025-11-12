#include <iostream>
#include <stdexcept>
#include <string>
#include "lumine/image.hpp"
#include "lumine/kernel.hpp"
#include "lumine/convolver.hpp"
#include "lumine/types.hpp"
#include "lumine/preprocessing.hpp"


using namespace lumine;


static void print_usage(){
    std::cout << "Usage: image_convolution <input> <output> --kernel <name|spec> [--stride N] [--padding zero|edge] [--grayscale]\n";
    std::cout << " Builtin kernels: identity, box3, box5, sharpen, sobel_x, sobel_y, gauss5\n";
    std::cout << " Custom spec example: \"1 0 -1; 1 0 -1; 1 0 -1\"\n";
}


int main(int argc, char** argv){
    if(argc < 5){ print_usage(); return 1; }
    std::string in = argv[1];
    std::string out = argv[2];


    std::string kernel_arg;
    int stride=1; Padding pad=Padding::ZERO; bool gray=false; VizMode viz=VizMode::Clamp;
    bool denoise = false, binarize = false;
    int window_size = 15;
    float k = 0.2f;


    for(int i=3;i<argc;++i){
        std::string a = argv[i];
        if(a=="--kernel" && i+1<argc){ kernel_arg = argv[++i]; }
        else if(a=="--stride" && i+1<argc){ stride = std::max(1, std::stoi(argv[++i])); }
        else if(a=="--padding" && i+1<argc){ std::string p=argv[++i]; pad = (p=="edge" ? Padding::EDGE : Padding::ZERO); }
        else if(a=="--grayscale"){ gray=true; }
        else if(a=="--viz" && i+1<argc){ 
            std::string m = argv[++i];
            if(m == "normalize") viz = VizMode::Normalize;
            else if(m == "none") viz = VizMode::None;
            else viz = VizMode::Clamp;
        }
        else if (a == "--grayscale") { gray = true; }
        else if (a == "--denoise") { denoise = true; }
        else if (a == "--binarize") { binarize = true; }
        else if (a == "--grayscale-window-size" && i + 1 < argc) { window_size = std::stoi(argv[++i]); }
        else if (a == "--binarize-k" && i + 1 < argc) { k = std::stof(argv[++i]); }
        else { std::cerr << "Unknown arg: " << a << "\n"; print_usage(); return 1; }
    }
    if(kernel_arg.empty()) { std::cerr << "--kernel is required\n"; return 1; }


    try{
        Image img = Image::load(in, gray);
        if (denoise) img = Preprocessing::denoise(img);
        if (binarize) img = Preprocessing::sauvola_binarization(img, k, window_size);
        if (gray) img = Preprocessing::grayscale(img);
        Kernel K;
        try { K = Kernel::from_builtin(kernel_arg); }
        catch(...) { K = Kernel::from_string(kernel_arg); }


        ConvParams params; params.stride=stride; params.padding=pad; params.viz=viz;
        Image outimg = Convolver::convolve(img, K, params);
        outimg.save(out);
        std::cout << "Wrote: " << out << " (" << outimg.width() << "x" << outimg.height() << ", c=" << outimg.channels() << ")\n";
        return 0;
    } catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
}
