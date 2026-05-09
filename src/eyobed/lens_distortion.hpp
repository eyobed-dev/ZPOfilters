#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;

// Lens Distortion 
Mat applyLensDistortion(const Mat& src) {
    Mat dst = Mat::zeros(src.size(), src.type());

    float k  = 5.0f;                                  // distortion coefficient
    float cx = src.cols / 2.0f;                        // center of the image
    float cy = src.rows / 2.0f;                        // center of the image
    float R  = sqrt(cx * cx + cy * cy);          // center-to-corner radius

    for (int y = 0; y < dst.rows; y++) {
        for (int x = 0; x < dst.cols; x++) {
            // Normalize it so that distotion is the same for different sizes of images
            float dx = (x - cx) / R;
            float dy = (y - cy) / R;

            float distortion = 1.0f + k * (dx * dx + dy * dy);

            // undo normalization to get the distorted coordinates back to normal
            int src_x = cvRound(dx * distortion * R + cx);
            int src_y = cvRound(dy * distortion * R + cy);

            // Bound check otherwise seg fault may happen
            if (src_x >= 0 && src_x < src.cols &&
                src_y >= 0 && src_y < src.rows)
                dst.at<Vec3b>(y, x) = src.at<Vec3b>(src_y, src_x);
        }
    }

    return dst;
}
