/**
 * @file      histogram_eq.hpp
 * @author    Eyobed Awel Nuri (xnuriey00)
 * @date      May 2026
 * @brief     Histogram Equalization filter implementation.
 * 
 * @copyright Copyright (c) 2026
 *            Project: ZPOe Image Filter Effects
 *            Brno University of Technology (BUT)
 */

#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

// Histogram Equalization

Mat applyHistogramEq(const Mat& src) {
    // BGR to YCrCb we need only Y for this
    Mat ycrcb;
    cvtColor(src, ycrcb, COLOR_BGR2YCrCb);

    int totalPixels = ycrcb.rows * ycrcb.cols;

    // make frequency histogram of Y values
    int hist[256] = {0};
    for (int y = 0; y < ycrcb.rows; y++)
        for (int x = 0; x < ycrcb.cols; x++)
    // increment the count of the pixel value
            hist[ycrcb.at<Vec3b>(y, x)[0]]++;

    // PDF
    float pdf[256] = {0.0f};
    for (int i = 0; i < 256; i++)
        pdf[i] = (float)hist[i] / totalPixels;

    // CDF 
    float cdf[256] = {0.0f};
    cdf[0] = pdf[0];
    for (int i = 1; i < 256; i++)
        cdf[i] = cdf[i - 1] + pdf[i];

    // Remap table scale CDF to [0, 255]
    int remap[256] = {0};
    for (int i = 0; i < 256; i++)
        remap[i] = std::max(0, std::min(255, cvRound(cdf[i] * 255.0f)));

    // Apply remap to Y channel only
    for (int y = 0; y < ycrcb.rows; y++)
        for (int x = 0; x < ycrcb.cols; x++)
            ycrcb.at<Vec3b>(y, x)[0] = remap[ycrcb.at<Vec3b>(y, x)[0]];

    // YCrCb to BGR convert back
    Mat dst;
    cvtColor(ycrcb, dst, COLOR_YCrCb2BGR);
    return dst;
}
