#pragma once
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace cv;

inline double gOrtonSigma = 2.0;
inline double gOrtonStrength = 0.6;

inline void setOrtonSigma(double sigma) {
    gOrtonSigma = std::max(0.1, std::min(20.0, sigma));
}

inline void setOrtonStrength(double strength) {
    gOrtonStrength = std::max(0.0, std::min(1.0, strength));
}

Mat applyOrton(const Mat& src) {
    Mat out = src.clone();

    const int rows = src.rows;
    const int cols = src.cols;

    Mat src64(rows, cols, CV_64FC3);
    Mat temp(rows, cols, CV_64FC3);
    Mat blur(rows, cols, CV_64FC3);

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            Vec3b p = src.at<Vec3b>(y, x);
            src64.at<Vec3d>(y, x) = Vec3d(p[0], p[1], p[2]);
        }
    }

    const double sigma = gOrtonSigma;
    const int radius = std::max(1, static_cast<int>(std::ceil(3.0 * sigma)));
    const int ksize = 2 * radius + 1;
    std::vector<double> kernel(ksize);

    double wsum = 0.0;
    for (int i = -radius; i <= radius; ++i) {
        double w = std::exp(-(i * i) / (2.0 * sigma * sigma));
        kernel[i + radius] = w;
        wsum += w;
    }
    for (double& w : kernel) w /= std::max(1e-12, wsum);

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            Vec3d acc(0.0, 0.0, 0.0);
            for (int k = -radius; k <= radius; ++k) {
                int nx = std::max(0, std::min(cols - 1, x + k));
                acc += src64.at<Vec3d>(y, nx) * kernel[k + radius];
            }
            temp.at<Vec3d>(y, x) = acc;
        }
    }

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            Vec3d acc(0.0, 0.0, 0.0);
            for (int k = -radius; k <= radius; ++k) {
                int ny = std::max(0, std::min(rows - 1, y + k));
                acc += temp.at<Vec3d>(ny, x) * kernel[k + radius];
            }
            blur.at<Vec3d>(y, x) = acc;
        }
    }

    const double strength = gOrtonStrength;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            Vec3d s = src64.at<Vec3d>(y, x);
            Vec3d b = blur.at<Vec3d>(y, x);
            Vec3b& dst = out.at<Vec3b>(y, x);

            for (int c = 0; c < 3; ++c) {
                double screen = 255.0 - ((255.0 - s[c]) * (255.0 - b[c]) / 255.0);
                double mixed = (1.0 - strength) * s[c] + strength * screen;
                int val = static_cast<int>(std::round(mixed));
                dst[c] = static_cast<uchar>(std::max(0, std::min(255, val)));
            }
        }
    }

    return out;
}
