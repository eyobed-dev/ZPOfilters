#ifndef SEPIA_HPP
#define SEPIA_HPP

#include <opencv2/opencv.hpp>
using namespace cv;

// ─────────────────────────────────────────────────────────────────────────────
//  HOW TO MAKE YOUR OWN FILTER — use this file as a template:
//
//    1. Copy this file to   include/yourname/yourfilter.hpp
//    2. Rename the function from  applySepia  to  applyYourFilter
//    3. Replace the logic inside the function with your own
//    4. Follow the 2 steps in src/main.cpp  (include + register with a description)
// ─────────────────────────────────────────────────────────────────────────────

// Applies a sepia (warm vintage brownish) tone to the image
Mat applySepia(const Mat& src) {

    Mat dst;

    // The sepia effect works by mixing each pixel's BGR channels using a 3x3 matrix.
    // Each row says: new_channel = (weight * B) + (weight * G) + (weight * R)
    Mat kernel = (Mat_<float>(3, 3) <<
        0.272, 0.534, 0.131,   // new Blue
        0.349, 0.686, 0.168,   // new Green
        0.393, 0.769, 0.189);  // new Red

    transform(src, dst, kernel);

    return dst;
}

#endif // SEPIA_HPP
