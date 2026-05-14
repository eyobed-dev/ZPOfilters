/**
 * @file      threshold.hpp
 * @author    Deniz Aksoy (xaksoyd00)
 * @date      May 2026
 * @brief     Threshold filter implementation.
 * 
 * @copyright Copyright (c) 2026
 *            Project: ZPOe Image Filter Effects
 *            Brno University of Technology (BUT)
 */

#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

Mat applyThreshold(const Mat& src) {
    Mat out = src.clone();
    int thresh = 127;
    for (int y = 0; y < out.rows; ++y) {
        for (int x = 0; x < out.cols; ++x) {
            Vec3b& p = out.at<Vec3b>(y, x);
            int intensity = 0.299 * p[2] + 0.587 * p[1] + 0.114 * p[0];
            if (intensity > thresh) {
                p[0] = 255; p[1] = 255; p[2] = 255;
            } else {
                p[0] = 0; p[1] = 0; p[2] = 0;
            }
        }
    }
    return out;
}
