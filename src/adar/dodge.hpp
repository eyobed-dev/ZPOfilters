#pragma once
#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace cv;

inline double gDodgeStrength = 1.0;
inline int gDodgeProtect = 0;

inline void setDodgeStrength(double strength) {
    gDodgeStrength = std::max(0.0, std::min(2.0, strength));
}

inline void setDodgeProtect(int protect) {
    gDodgeProtect = std::max(0, std::min(128, protect));
}

Mat applyDodge(const Mat& src) {
    Mat out = src.clone();
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            Vec3b p = src.at<Vec3b>(y, x);
            Vec3b& out_p = out.at<Vec3b>(y, x);
            for (int c = 0; c < 3; ++c) {
                int dodgeVal = (p[c] * 255) / std::max(1, 255 - p[c] + gDodgeProtect);
                int blended = static_cast<int>(std::round((1.0 - gDodgeStrength) * p[c] + gDodgeStrength * dodgeVal));
                out_p[c] = std::max(0, std::min(255, blended));
            }
        }
    }
    return out;
}
