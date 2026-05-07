#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>

cv::Mat applyPosterize(const cv::Mat& src) {
    cv::Mat dst = src.clone();
    
    // Number of distinct color levels per channel
    int levels = 4; 
    
    if (levels < 2) levels = 2; // Minimum 2 levels (0 and 255)
    
    float step = 255.0f / (levels - 1);
    
    // Precompute lookup table for performance instead of doing math for every single pixel
    uchar lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = cv::saturate_cast<uchar>(std::round(std::round(i / step) * step));
    }
    
    int channels = src.channels();
    
    // Manually apply the posterize effect pixel by pixel
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            if (channels == 3) {
                cv::Vec3b p = src.at<cv::Vec3b>(y, x);
                p[0] = lut[p[0]]; // Blue
                p[1] = lut[p[1]]; // Green
                p[2] = lut[p[2]]; // Red
                dst.at<cv::Vec3b>(y, x) = p;
            } else if (channels == 1) {
                uchar p = src.at<uchar>(y, x);
                dst.at<uchar>(y, x) = lut[p];
            }
        }
    }
    
    return dst;
}
