# Lumine - Image Convolution Tool

## Build

```bash
mkdir -p external/stb
# download stb_image.h and stb_image_write.h into external/stb
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run (examples)

```bash
# Sobel edge magnitude approximation (combine x/y in future step)
./build/lumine input.jpg out_sobelx.png --kernel sobel_x --padding edge --grayscale


# Box blur 5x5 with stride 2 (downsample)
./build/lumine input.jpg out_blur.png --kernel box5 --stride 2 --padding edge


# Custom kernel via inline spec (3x3)
./build/lumine input.jpg out_custom.png --kernel "1 0 -1; 1 0 -1; 1 0 -1" --padding zero --grayscale
```

## Roadmap (incremental)

- [x] Project skeleton (CMake, core classes, CLI)
- [x] Basic convolution with stride & zero/edge padding
- [ ] Proper value range handling (keep signed output, auto-normalize visualization)
- [ ] Support separable kernels for speed (gaussian)
- [ ] Multi-threading (std::execution or OpenMP)
- [ ] Unit tests (Catch2/GoogleTest)
- [ ] Benchmarking harness
- [ ] PNG/JPG metadata passthrough (optional)
- [ ] Documentation site / examples gallery
