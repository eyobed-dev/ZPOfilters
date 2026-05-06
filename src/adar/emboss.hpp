#pragma once
#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace cv;

Mat applyEmboss(const Mat& src) {
    Mat out = src.clone();
    for (int y = 1; y < src.rows - 1; ++y) {
        for (int x = 1; x < src.cols - 1; ++x) {
            Vec3b p1 = src.at<Vec3b>(y - 1, x - 1);
            Vec3b p2 = src.at<Vec3b>(y + 1, x + 1);
            Vec3b& out_p = out.at<Vec3b>(y, x);
            for (int c = 0; c < 3; ++c) {
                int val = p1[c] - p2[c] + 128;
                out_p[c] = std::max(0, std::min(255, val));
            }
        }
    }
    return out;
}
