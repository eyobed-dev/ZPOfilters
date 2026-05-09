#pragma once
#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace cv;

inline int gBloomRadius = 3;
inline int gBloomThreshold = 180;
inline double gBloomStrength = 1.0;

inline void setBloomRadius(int radius) {
    gBloomRadius = std::max(1, std::min(12, radius));
}

inline void setBloomThreshold(int threshold) {
    gBloomThreshold = std::max(0, std::min(255, threshold));
}

inline void setBloomStrength(double strength) {
    gBloomStrength = std::max(0.0, std::min(3.0, strength));
}

Mat applyBloom(const Mat& src) {
    Mat out = src.clone();
    int r = gBloomRadius;

    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            int sum[3] = {0, 0, 0};
            int count = 0;
            
            for (int dy = -r; dy <= r; ++dy) {
                for (int dx = -r; dx <= r; ++dx) {
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < src.rows && nx >= 0 && nx < src.cols) {
                        Vec3b p = src.at<Vec3b>(ny, nx);
                        int intensity = 0.299 * p[2] + 0.587 * p[1] + 0.114 * p[0];
                        if (intensity > gBloomThreshold) {
                            sum[0] += p[0];
                            sum[1] += p[1];
                            sum[2] += p[2];
                            count++;
                        }
                    }
                }
            }
            
            if (count > 0) {
                Vec3b p = src.at<Vec3b>(y, x);
                Vec3b& out_p = out.at<Vec3b>(y, x);
                int area = (2 * r + 1) * (2 * r + 1);
                for (int c = 0; c < 3; ++c) {
                    int glow = sum[c] / area;
                    int val = static_cast<int>(std::round(p[c] + gBloomStrength * glow));
                    out_p[c] = std::max(0, std::min(255, val));
                }
            }
        }
    }
    return out;
}
