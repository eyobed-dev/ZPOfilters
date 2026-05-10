# Application of Basic Image Processing Filters on Human Face

## Team
- Adar Otieno - xotiena00
- Deniz Aksoy - xaksoyd00
- Eyobed Awel Nuri - xnuriey00

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

### Linux (Merlin)
```bash
sudo apt update
sudo apt install cmake libopencv-dev
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
./project
```
## How to Use

- A **Controls** window will open with trackbars.
- Use the **Image** trackbar to switch between images found in the `data/` folder.
- Use the **Filter** trackbar to switch between different effects.
- The active filter's name and description are shown in the **Info** window.
- Press any key with a window focused to exit.
- If you cannot see the windows, drag them around to expose them.

---

