/**
 * @file      vignette_blur.hpp
 * @author    Eyobed Awel Nuri (xnuriey00)
 * @date      May 2026
 * @brief     Vignette Blur filter implementation.
 * 
 * @copyright Copyright (c) 2026
 *            Project: ZPOe Image Filter Effects
 *            Brno University of Technology (BUT)
 */

#pragma once
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat applyVignetteBlur(const Mat &src) {
  Mat dst = src.clone();

  float cx = src.cols / 2.0f;
  float cy = src.rows / 2.0f;
  float R  = sqrt(cx * cx + cy * cy); // Center-to-corner radius

  // Edge avoiding loops
  for (int y = 7; y < src.rows - 7; y++) {
    for (int x = 7; x < src.cols - 7; x++) {

      // Normalize radial distance from center (0.0 to 1.0)
      float dx = x - cx;
      float dy = y - cy;
      float dist = min(1.0f, sqrt(dx * dx + dy * dy) / R);

      // 15x15 Box Blur convolution
      int sumB = 0, sumG = 0, sumR = 0;
      for (int dy_off = -7; dy_off <= 7; dy_off++) {
        for (int dx_off = -7; dx_off <= 7; dx_off++) {
          Vec3b n = src.at<Vec3b>(y + dy_off, x + dx_off);
          sumB += n[0]; sumG += n[1]; sumR += n[2];
        }
      }
      Vec3b blur(sumB / 225, sumG / 225, sumR / 225);
      Vec3b orig = src.at<Vec3b>(y, x);

        //Blend original with blurred image based on distance from center
      dst.at<Vec3b>(y, x) = Vec3b(
        orig[0] * (1.0f - dist) + blur[0] * dist,
        orig[1] * (1.0f - dist) + blur[1] * dist,
        orig[2] * (1.0f - dist) + blur[2] * dist
      );
    }
  }
 //finished
  return dst;
}
