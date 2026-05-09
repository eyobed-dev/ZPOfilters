#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

// Posterize
Mat applyPosterize(const Mat& src) {
    Mat dst = src.clone();

    int levels = 8;
    if (levels < 2) levels = 2;

    // Calculate the step size for the quantization
    float step = 255.0f / (levels - 1);

    // Create a Look-Up Table 
    uchar lut[256];
    for (int i = 0; i < 256; i++) {
        float level = cvRound(i / step);    
        float intensity = level * step;         
        lut[i] = (uchar)(cvRound(intensity)); 
    }

    // Apply LUT to each pixel(BGR)
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            Vec3b p = src.at<Vec3b>(y, x);
            p[0] = lut[p[0]]; 
            p[1] = lut[p[1]]; 
            p[2] = lut[p[2]]; 
                dst.at<Vec3b>(y, x) = p;

        }
    }

    return dst;
}
