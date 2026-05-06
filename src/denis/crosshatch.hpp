#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

Mat applyCrosshatch(const Mat& src) {
    Mat out(src.size(), CV_8UC3, Scalar(255, 255, 255));
    int step = 4;
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            Vec3b p = src.at<Vec3b>(y, x);
            int intensity = 0.299 * p[2] + 0.587 * p[1] + 0.114 * p[0];
            
            bool draw = false;
            if (intensity < 60) {
                if ((x + y) % step == 0 || (x - y) % step == 0 || (x + y - 2) % step == 0 || (x - y - 2) % step == 0) draw = true;
            } else if (intensity < 120) {
                if ((x + y) % step == 0 || (x - y) % step == 0) draw = true;
            } else if (intensity < 180) {
                if ((x + y) % step == 0) draw = true;
            } else if (intensity < 220) {
                if ((x + y) % (step * 2) == 0) draw = true;
            }
            
            if (draw) {
                out.at<Vec3b>(y, x) = Vec3b(0, 0, 0);
            }
        }
    }
    return out;
}
