#pragma once
#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace cv;

Mat applyDodge(const Mat& src) {
    Mat out = src.clone();
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            Vec3b p = src.at<Vec3b>(y, x);
            Vec3b& out_p = out.at<Vec3b>(y, x);
            for (int c = 0; c < 3; ++c) {
                int val = (p[c] * 255) / std::max(1, 255 - p[c]);
                out_p[c] = std::max(0, std::min(255, val));
            }
        }
    }
    return out;
}
