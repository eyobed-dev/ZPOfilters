#ifndef ADAR_MEDIAN_HPP
#define ADAR_MEDIAN_HPP

#include <opencv2/opencv.hpp>
using namespace cv;

// Median Filter — removes noise by replacing each pixel with the
// median value of its neighbors.
//
// Unlike averaging (Gaussian blur), the median completely ignores
// outlier pixel values. This makes it very effective at removing
// "salt and pepper" noise (random black/white dots) while keeping
// edges sharp.
//
// The kernel size (5) means a 5×5 neighborhood is used.
// Must be an odd number: 3, 5, 7, 9 ...  (larger = stronger effect)
Mat applyMedian(const Mat& src) {

    Mat dst;
    medianBlur(src, dst, 5);

    return dst;
}

#endif // ADAR_MEDIAN_HPP
