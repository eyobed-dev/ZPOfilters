# Application of Basic Image Processing Filters on Human Face
Images
---

## Directory Structure

```
ZPOe_Project_Filter_Effects/
├── data/       input images here (.jpg, .png, etc.)
├── src/        main.cpp and filter implementations
│   ├── eyobed/
│   ├── adar/   README: Python filters (Bloom, Orton, Gamma, Dodge) + demo links
│   └── denis/
├── build/      CMake build directory
```
 
---


## Prerequisites

Install **CMake** and **OpenCV** before building.

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install cmake libopencv-dev
```

### macOS
```bash
brew install cmake opencv
```

---

## How to Build

```bash
mkdir -p build
cd build
cmake ..
make
```

---

## How to Run

1. Make sure you have images inside the `data/` folder.
2. Run the compiled binary from the `build` directory:

```bash
cd build
./ZPOe_Filter_Effects
```

- A **Controls** window will open with trackbars.
- Use the **Image** trackbar to switch between images found in the `data/` folder.
- Use the **Filter** trackbar to switch between different effects.
- The active filter's name and description are shown in the **Info** window.
- Press any key with a window focused to exit.

---

