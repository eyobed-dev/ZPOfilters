#ifndef EYOBED_SOBEL_HPP
#define EYOBED_SOBEL_HPP

#include <opencv2/opencv.hpp>
using namespace cv;

// Detects edges using the Sobel operator.
//
// The Sobel operator computes the image gradient — how fast pixel
// values change at each location. Sharp changes = edges.
// It computes gradients in X (vertical edges) and Y (horizontal edges)
// separately, then combines them.
Mat applySobel(const Mat& src) {

    // Edges are intensity changes — work in grayscale
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // Compute gradient in X direction (finds vertical edges)
    // and Y direction (finds horizontal edges)
    // CV_16S: use signed 16-bit to avoid overflow from the kernel
    Mat gradX, gradY;
    Sobel(gray, gradX, CV_16S, 1, 0, 3);
    Sobel(gray, gradY, CV_16S, 0, 1, 3);

    // Convert to absolute 8-bit values (edges can be negative or positive)
    Mat absX, absY;
    convertScaleAbs(gradX, absX);
    convertScaleAbs(gradY, absY);

    // Combine: final edge strength ≈ magnitude of the gradient vector
    Mat dst;
    addWeighted(absX, 0.5, absY, 0.5, 0, dst);

    return dst;
}

#endif // EYOBED_SOBEL_HPP
