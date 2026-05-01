#ifndef EYOBED_VIGNETTE_HPP
#define EYOBED_VIGNETTE_HPP

#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

// Vignette — darkens the edges of the image while keeping the center bright.
//
// A 2D Gaussian surface is used as a brightness mask:
// value = 1.0 at the center, falling smoothly toward 0.0 at the edges.
// Each pixel is multiplied by this mask value.
Mat applyVignette(const Mat& src) {

    // Build 1D Gaussian curves for each axis.
    // sigma controls how wide the bright area is (larger = wider)
    double sigma = src.cols * 0.45;
    Mat kernelX = getGaussianKernel(src.cols, sigma, CV_32F);
    Mat kernelY = getGaussianKernel(src.rows, sigma, CV_32F);

    // Outer product of the two 1D curves → 2D Gaussian surface
    Mat mask = kernelY * kernelX.t();

    // Normalize so the center = 1.0 and the corners approach 0.0
    normalize(mask, mask, 0.0, 1.0, NORM_MINMAX);

    // Apply the mask to each BGR channel separately
    Mat dst = src.clone();
    vector<Mat> channels;
    split(dst, channels);

    for (int i = 0; i < 3; i++) {
        Mat chF;
        channels[i].convertTo(chF, CV_32F);   // work in float
        multiply(chF, mask, chF);              // darken by mask
        chF.convertTo(channels[i], CV_8U);     // back to 8-bit
    }

    merge(channels, dst);
    return dst;
}

#endif // EYOBED_VIGNETTE_HPP
