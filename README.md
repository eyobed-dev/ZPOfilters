# ZPOe Project: Filter Effects in Image



---

## Directory Structure

```
ZPOe_Project_Filter_Effects/
├── data/        ← Put your input images here (.jpg, .png, etc.)
├── src/         ← main.cpp and filter implementations
│   ├── eyobed/
│   ├── adar/
│   └── denis/
├── build/       ← CMake build directory
```

---

## Implemented Filters

The following filters are currently available in the application:

| Filter Name | Description | Author |
|---|---|---|

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

## How to Add Your Own Filter

### Step 1 — Create your filter file

Create a new file in `src/yourname/yourfilter.hpp`.
Inside it, implement your logic:

```cpp
#pragma once
#include <opencv2/opencv.hpp>

cv::Mat applyYourFilter(const cv::Mat& src) {
    cv::Mat dst;
    // your image processing code here
    return dst;
}
```

### Step 2 — Register it in `src/main.cpp`

**1. Include your file:**
```cpp
//  STEP 1 — Include your filter hpp file here
#include "yourname/yourfilter.hpp"
```

**2. Add it to the list:**
```cpp
//  STEP 2 — Register your filter here
Filter allFilters[] = {
    // ... existing filters
    { "Your Filter Name",
      "Description of what your filter does.\nUse newlines for formatting.",
      applyYourFilter },
};
```

Re-run `make` inside the `build` directory to compile your changes.
