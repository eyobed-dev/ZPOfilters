#ifndef DENIS_NOISE_HPP
#define DENIS_NOISE_HPP

#include <opencv2/opencv.hpp>
using namespace cv;

// Gaussian Noise — adds random noise to every pixel to simulate
// camera sensor noise or film grain.
//
// Each pixel gets a random offset drawn from a Gaussian distribution
// (bell curve) with mean=0 and stddev=25.
// Mean=0 means the noise is equally likely to brighten or darken.
// Larger stddev = stronger / more visible noise.
Mat applyGaussianNoise(const Mat& src) {

    // Work in float so we can handle negative noise values without overflow
    Mat srcFloat;
    src.convertTo(srcFloat, CV_32FC3);

    // Generate random noise: same size/type as the image, mean=0, stddev=25
    Mat noise(src.size(), CV_32FC3);
    randn(noise, 0.0, 25.0);

    // Add the noise to the image
    Mat result;
    add(srcFloat, noise, result);

    // Convert back to 8-bit — values outside [0, 255] are clamped automatically
    result.convertTo(result, CV_8UC3);

    return result;
}

#endif // DENIS_NOISE_HPP
