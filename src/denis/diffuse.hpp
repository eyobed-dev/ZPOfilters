#pragma once
#include <opencv2/opencv.hpp>
#include <cstdlib>

using namespace cv;

Mat applyDiffuse(const Mat& src) {
    Mat out = src.clone();
    int R = 5;
    for (int y = 0; y < out.rows; ++y) {
        for (int x = 0; x < out.cols; ++x) {
            int dx = (rand() % (2 * R + 1)) - R;
            int dy = (rand() % (2 * R + 1)) - R;
            
            int nx = std::max(0, std::min(out.cols - 1, x + dx));
            int ny = std::max(0, std::min(out.rows - 1, y + dy));
            
            out.at<Vec3b>(y, x) = src.at<Vec3b>(ny, nx);
        }
    }
    return out;
}
