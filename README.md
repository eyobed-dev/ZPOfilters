# ZPOe Project: Filter Effects in Image

This project implements a set of aesthetic image fiter.

##  Structure
- `data/`: Input images for testing.
- `doc/`: Project proposal and documentation.
- `include/`: C++ header files.
- `src/`: Source code implementation safely.
- `output/`: Processed image results.

## Filters
Based on the course requirements, the following filters are suggested for implementation:

add them here

## How to Build
This project uses **CMake** for build management.

```bash
mkdir build
cd build
cmake ..
make
```

##  Collaboration 
To avoid merge conflicts follow these steps to add a new filter okey

1. **Create a header file** in `include/` (e.g., `MyFilter.hpp`).
2. **Inherit from `Filter`** and implement `getName()` and `apply()`.
3. **Register your filter** in `src/main.cpp`:
   - Include your header: `#include "MyFilter.hpp"`
   - Add to the registry: `filters.push_back(std::make_shared<MyFilter>());`

### Coding style
- **Class Names**: PascalCase (e.g., `PaintingFilter`).
- **Method Names**: camelCase (e.g., `apply()`).
- **Variables**: camelCase or snake_case consistently.
- **Documentation**: Comment your logic inside the `apply()` method.

enjoy
