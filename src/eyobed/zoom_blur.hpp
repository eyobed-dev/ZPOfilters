#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat applyZoomBlur(const cv::Mat& src) {
    cv::Mat dst = cv::Mat::zeros(src.size(), src.type());
    
    // Radius controls the number of samples (strength of the blur)
    int radius = 15; 
    float blurStrength = 0.05f; // Scale distance controls how far it zooms
    
    float cx = src.cols / 2.0f;
    float cy = src.rows / 2.0f;
    
    int channels = src.channels();
    
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            std::vector<int> sum(channels, 0);
            int count = 0;
            
            // Manual application of a 1D box blur kernel along the radial vector
            for (int i = 0; i <= radius; i++) {
                float scale = 1.0f - (float)i / (float)radius * blurStrength;
                
                int sx = cvRound(cx + (x - cx) * scale);
                int sy = cvRound(cy + (y - cy) * scale);
                
                if (sx >= 0 && sx < src.cols && sy >= 0 && sy < src.rows) {
                    if (channels == 3) {
                        cv::Vec3b p = src.at<cv::Vec3b>(sy, sx);
                        sum[0] += p[0];
                        sum[1] += p[1];
                        sum[2] += p[2];
                    } else if (channels == 1) {
                        sum[0] += src.at<uchar>(sy, sx);
                    }
                    count++;
                }
            }
            
            if (channels == 3) {
                dst.at<cv::Vec3b>(y, x) = cv::Vec3b(sum[0] / count, sum[1] / count, sum[2] / count);
            } else if (channels == 1) {
                dst.at<uchar>(y, x) = cv::saturate_cast<uchar>(sum[0] / count);
            }
        }
    }
    
    return dst;
}
