#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>

cv::Mat applyLensDistortion(const cv::Mat& src) {
    cv::Mat dst = cv::Mat::zeros(src.size(), src.type());
    
    // Distortion parameter
    // Positive for barrel distortion, negative for pincushion
    float k = 0.5f; 
    
    float cx = src.cols / 2.0f;
    float cy = src.rows / 2.0f;
    
    // Max radius for normalization (distance from center to corner)
    float R = std::sqrt(cx * cx + cy * cy);
    
    int channels = src.channels();
    
    // Manual geometric transform (backward mapping to avoid holes)
    for (int y = 0; y < dst.rows; y++) {
        for (int x = 0; x < dst.cols; x++) {
            
            // Normalize coordinates (-1 to 1)
            float dx = (x - cx) / R;
            float dy = (y - cy) / R;
            
            float r2 = dx * dx + dy * dy;
            
            // Apply radial distortion formula
            float distortion = 1.0f + k * r2;
            
            // Mapped source coordinates
            float sx = dx * distortion * R + cx;
            float sy = dy * distortion * R + cy;
            
            // Nearest neighbor interpolation
            int src_x = cvRound(sx);
            int src_y = cvRound(sy);
            
            // Copy pixel if within bounds
            if (src_x >= 0 && src_x < src.cols && src_y >= 0 && src_y < src.rows) {
                if (channels == 3) {
                    dst.at<cv::Vec3b>(y, x) = src.at<cv::Vec3b>(src_y, src_x);
                } else if (channels == 1) {
                    dst.at<uchar>(y, x) = src.at<uchar>(src_y, src_x);
                }
            }
        }
    }
    
    return dst;
}
