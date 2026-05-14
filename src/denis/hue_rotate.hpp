/**
 * @file      hue_rotate.hpp
 * @author    Deniz Aksoy (xaksoyd00)
 * @date      May 2026
 * @brief     Hue Rotate filter implementation.
 * 
 * @copyright Copyright (c) 2026
 *            Project: ZPOe Image Filter Effects
 *            Brno University of Technology (BUT)
 */

#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

Mat applyHueRotate(const Mat& src) {
    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    int shift = 60;
    for (int y = 0; y < hsv.rows; ++y) {
        for (int x = 0; x < hsv.cols; ++x) {
            Vec3b& p = hsv.at<Vec3b>(y, x);
            p[0] = (p[0] + shift) % 180;
        }
    }
    Mat out;
    cvtColor(hsv, out, COLOR_HSV2BGR);
    return out;
}
