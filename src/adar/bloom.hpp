#pragma once
#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace cv;

Mat applyBloom(const Mat& src) {
    Mat out = src.clone();
    int r = 3;

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
                        if (intensity > 180) {
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
                    int val = p[c] + (sum[c] / area);
                    out_p[c] = std::max(0, std::min(255, val));
                }
            }
        }
    }
    return out;
}
