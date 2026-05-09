#pragma once
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cmath>

using namespace cv;

inline double gGammaValue = 2.2;

inline void setGammaValue(double gamma) {
    gGammaValue = std::max(0.05, std::min(5.0, gamma));
}

Mat applyGamma(const Mat& src) {
    Mat out = src.clone();

    // Gamma > 1.0 darkens, Gamma < 1.0 brightens.
    const double gamma = gGammaValue;
    const double invGamma = 1.0 / std::max(1e-6, gamma);

    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            Vec3b p = src.at<Vec3b>(y, x);
            Vec3b& out_p = out.at<Vec3b>(y, x);

            for (int c = 0; c < 3; ++c) {
                double normalized = p[c] / 255.0;
                int corrected = static_cast<int>(std::round(std::pow(normalized, invGamma) * 255.0));
                out_p[c] = static_cast<uchar>(std::max(0, std::min(255, corrected)));
            }
        }
    }

    return out;
}
