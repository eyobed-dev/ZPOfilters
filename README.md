# ZPOe Project: Filter Effects in Image

This project applies image filters using **C++** and **OpenCV**.
Results are saved automatically to the `output/` folder.

---

## Directory Structure

```
ZPOe_Project_Filter_Effects/
├── data/        ← Put your input images here (.jpg, .png, etc.)
├── include/     ← One .hpp file per filter (plain functions — no classes needed)
├── src/         ← main.cpp — only the top section needs editing
├── output/      ← Filtered results are saved here automatically
└── build/       ← CMake build files (generated, do not commit)
```

---

## Implemented Filters

| # | File | Function | Description |
|---|---|---|---|
| 0 | `sepia.hpp` | `applySepia` | Warm brownish vintage tone |

> Add your row to this table when you implement a filter.

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

The compiled binary will be at `build/ZPOe_Filter_Effects`.

---

## How to Run

```bash
./build/ZPOe_Filter_Effects data/your_image.jpg
```

- The menu lists all registered filters.
- If only one filter exists it runs automatically.
- The result is saved to `output/` **and** shown in a window.
- Press any key in the image window to exit.

---

## How to Add Your Own Filter

### Step A — Create your filter file

Copy `include/sepia.hpp` to `include/yourfiltername.hpp`.

Inside it, rename the function and replace the logic:

```cpp
Mat applyYourFilter(Mat src) {

    Mat dst;

    // your image processing code here
    // dst = ...

    return dst;
}
```

### Step B — Register it in `src/main.cpp`

The file has two clearly marked places at the top. You only touch those two.

**1. Include your file:**
```cpp
#include "yourfiltername.hpp"
```

**2. Add it to the list:**
```cpp
Filter allFilters[] = {
    { "Sepia Tone",   applySepia        },
    { "Your Filter",  applyYourFilter   },   // <-- add this line
};
```

That's it. The rest of `main.cpp` handles everything else.

---

## Coding Style

Match the style used in ZPOe course exercises:

| Item | Convention | Example |
|---|---|---|
| File names | lowercase with no spaces | `edgedetect.hpp` |
| Function names | camelCase with `apply` prefix | `applyEdgeDetect` |
| Variables | camelCase | `srcImage`, `blurResult` |
| Comments | explain non-obvious logic | `// kernel is flipped for true convolution` |

---

## Useful OpenCV functions

| Function | What it does |
|---|---|
| `cvtColor(src, dst, COLOR_BGR2GRAY)` | Convert to grayscale |
| `GaussianBlur(src, dst, Size(k,k), sigma)` | Gaussian blur |
| `Canny(src, dst, low, high)` | Edge detection |
| `threshold(src, dst, thresh, max, type)` | Thresholding |
| `transform(src, dst, kernel)` | Apply a colour matrix |
| `filter2D(src, dst, -1, kernel)` | Apply a convolution kernel |
