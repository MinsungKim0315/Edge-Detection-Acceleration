# Edge Detector

This project implements a full Canny-style edge detector in C++ without third-party libraries.

## Structure

```text
Final_Project/
├── CMakeLists.txt
├── README.md
├── image/
│   ├── bird.png
│   └── mona_lisa.png
├── include/
│   ├── gaussian.hpp
│   ├── hysteresis.hpp
│   ├── image.hpp
│   ├── image_io.hpp
│   ├── nms.hpp
│   ├── sobel.hpp
│   └── threshold.hpp
├── input/
│   ├── bird.pgm
│   └── mona_lisa.pgm
├── output/
│   ├── bird_edges.pgm
│   ├── bird_edges.png
│   ├── mona_lisa_edge.pgm
│   └── mona_lisa_edge.png
└── src/
    ├── gaussian.cpp
    ├── hysteresis.cpp
    ├── image_io.cpp
    ├── main.cpp
    ├── nms.cpp
    ├── sobel.cpp
    └── threshold.cpp
```

## Supported input/output

- Input: `PGM` grayscale images (`P2` ASCII or `P5` binary)
- Output: `PGM` grayscale edge image

`PGM` was chosen deliberately because this project avoids third-party libraries and needs full visibility into each processing stage. Compared with formats like `PNG` or `JPEG`, `PGM` is simple to parse manually, maps directly to grayscale edge detection, and keeps the implementation focused on the actual algorithm rather than external image-decoding complexity.

## Build

```powershell
cmake -S . -B build
cmake --build build
```

## Run

```powershell
.\build\edge_detector.exe input.pgm output.pgm --low 40 --high 100 --kernel 5 --sigma 1.4
```

WSL/bash:

```bash
./edge_detector input.pgm output.pgm --low 40 --high 100 --kernel 5 --sigma 1.4
```

Example:

```bash
./edge_detector input/bird.pgm output/bird_edges.pgm --low 40 --high 100 --kernel 5 --sigma 1.4
```

In this example:

- Input image: `input/bird.pgm`
- Output edge image: `output/bird_edges.pgm`

Input image:

![Bird input](image/bird.png)

Output edge image:

![Bird edges](output/bird_edges.png)

Second example:

```bash
./edge_detector input/mona_lisa.pgm output/mona_lisa_edge.pgm --low 40 --high 100 --kernel 5 --sigma 1.4
```

In this example:

- Input image: `input/mona_lisa.pgm`
- Output edge image: `output/mona_lisa_edge.pgm`

Input image:

![Mona Lisa input](image/mona_lisa.png)

Output edge image:

![Mona Lisa edges](output/mona_lisa_edge.png)

Optional intermediate dumps:

```powershell
.\build\edge_detector.exe input.pgm output.pgm --dump-prefix outputs\step
```

WSL/bash:

```bash
./edge_detector input.pgm output.pgm --dump-prefix outputs/step
```

This will emit:

- `outputs\step_blur.pgm`
- `outputs\step_gradient.pgm`
- `outputs\step_nms.pgm`
- `outputs\step_threshold.pgm`

## Notes

- No OpenCV or external image-processing libraries are used.
- The implementation is split into explicit pipeline stages for inspection and future acceleration work.
