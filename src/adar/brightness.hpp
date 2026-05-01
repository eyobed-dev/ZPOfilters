#ifndef ADAR_BRIGHTNESS_HPP
#define ADAR_BRIGHTNESS_HPP

#include <opencv2/opencv.hpp>
using namespace cv;

// Brightness — adds a fixed offset to every pixel value.
//
// convertTo(output, type, scale, offset):
//   type   = -1  → keep the same pixel type as input
//   scale  = 1.0 → no scaling (multiply each pixel by 1)
//   offset = 60  → add 60 to every pixel (positive = brighter)
//
// Values that would exceed 255 are automatically clamped to 255.
// Values that would go below 0 are clamped to 0.
// Change 60 to a negative number to darken instead.
Mat applyBrightness(const Mat& src) {

    Mat dst;
    src.convertTo(dst, -1, 1.0, 60);

    return dst;
}

#endif // ADAR_BRIGHTNESS_HPP
