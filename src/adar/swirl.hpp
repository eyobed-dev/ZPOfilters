#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>
#include <algorithm>

using namespace cv;

Mat applySwirl(const Mat& src) {
    Mat out = src.clone();
    double cx = src.cols / 2.0;
    double cy = src.rows / 2.0;
    double radius = std::min(cx, cy);
    double amount = 3.14159;

    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            double dx = x - cx;
            double dy = y - cy;
            double r = std::sqrt(dx * dx + dy * dy);
            
            if (r < radius) {
                double theta = std::atan2(dy, dx);
                theta += amount * (radius - r) / radius;
                
                int nx = std::round(cx + r * std::cos(theta));
                int ny = std::round(cy + r * std::sin(theta));
                
                nx = std::max(0, std::min(src.cols - 1, nx));
                ny = std::max(0, std::min(src.rows - 1, ny));
                
                out.at<Vec3b>(y, x) = src.at<Vec3b>(ny, nx);
            }
        }
    }
    return out;
}
