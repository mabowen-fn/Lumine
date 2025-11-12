# Lumine - Image Convolution Tool

## Overview

**Lumine** is an image processing tool designed to perform various convolution operations (such as edge detection, blurring, etc.) on images. The tool supports **separable kernels** for performance optimization and provides a flexible command-line interface for easy use.

### New Features:
- **Preprocessing:** Added grayscale conversion, denoising (using a median filter), and Sauvola binarization.
- **Separable Kernels:** Optimized convolution for separable kernels (e.g., Gaussian blur).
- **Multi-threading** (upcoming): Future support for parallel processing to speed up convolutions.
  
This tool is a foundational component for building more complex applications like **OCR** (optical character recognition) and **image analysis**.

---

## Build Instructions

1. Create the `external/stb` directory:
    ```bash
    mkdir -p external/stb
    ```
2. Download `stb_image.h` and `stb_image_write.h` into the `external/stb` directory.
3. Use CMake to build the project:
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j
    ```

---

## Run (examples)

Lumine supports several operations like **convolution**, **preprocessing**, and **visualization**. Here are some example commands:

### **Convolution Operations:**
```bash
# Separable path (gauss5 is separable): much faster via 1D passes
./lumine input.jpg out_gauss.png --kernel gauss5 --padding edge --viz normalize

# Non-separable path (sobel is NOT strictly separable; uses 2D fallback)
./lumine input.jpg out_sobel.png --kernel sobel_x --padding edge --viz normalize

# Sobel edge magnitude approximation (combine x/y in future step)
./build/lumine input.jpg out_sobelx.png --kernel sobel_x --padding edge --grayscale

# Strided separable convolution (downsample horizontally & vertically)
./lumine input.jpg out_down.png --kernel gauss5 --stride 2 --padding edge --viz clamp

# Box blur 5x5 with stride 2 (downsample)
./build/lumine input.jpg out_blur.png --kernel box5 --stride 2 --padding edge

# Normalize (recommended for filters with negative responses)
./lumine input.jpg out_norm.png --kernel sobel_x --padding edge --viz normalize

# Custom kernel via inline spec (3x3)
./build/lumine input.jpg out_custom.png --kernel "1 0 -1; 1 0 -1; 1 0 -1" --padding zero --grayscale
```

### **Preprocessing Operations:**
```bash
# Grayscale + Denoise (using a median filter)
./lumine input.jpg out_grayscale_denoise.png --grayscale --denoise

# Sauvola binarization (adaptive thresholding)
./lumine input.jpg out_binarized.png --binarize --binarize-k 0.3

# Combined preprocessing: Grayscale → Denoise → Binarization
./lumine input.jpg out_preprocessed.png --grayscale --denoise --binarize --binarize-k 0.3
```

---

## Roadmap (incremental)

| Stage             | Must-have algorithms                             | Optional advanced                    |
| ----------------- | ------------------------------------------------ | ------------------------------------ |
| **Preprocessing**     | grayscale, denoise, Sauvola binarization, deskew | dewarp (polyfit), background removal |
| **Layout**            | CCA (Connected Component Analysis), projection profiles | U-Net segmentation                   |
| **Line segmentation** | horizontal projection                            | seam carving                         |
| **Recognition**       | CNN + CTC                                        | CNN+BiLSTM, Transformer OCR          |
| **LM**                | n-gram KenLM integration                         | Transformer LM                       |
| **Postprocess**       | Unicode normalization, diacritic fix             | morphological analysis               |

### Completed:
- [x] **Project skeleton** (CMake, core classes, CLI)
- [x] **Basic convolution** with stride & zero/edge padding
- [x] **Proper value range handling** (keep signed output, auto-normalize visualization)
- [x] **Preprocessing** (grayscale, denoise, Sauvola binarization)
- [x] **Support separable kernels** for speed (Gaussian)
- [ ] **Multi-threading** (std::execution or OpenMP)
- [ ] **Unit tests** (Catch2/GoogleTest)
- [ ] **Benchmarking harness**
- [ ] **PNG/JPG metadata passthrough** (optional)
- [ ] **Documentation site / examples gallery**

---
