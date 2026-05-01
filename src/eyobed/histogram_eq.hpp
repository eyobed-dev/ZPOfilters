#ifndef EYOBED_HISTOGRAM_EQ_HPP
#define EYOBED_HISTOGRAM_EQ_HPP

#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

// Histogram Equalization — improves contrast by spreading pixel
// intensity values more evenly across the full 0–255 range.
//
// Done in YCrCb color space so only brightness (Y channel) is equalized,
// not the colors — this avoids ugly color shifts.
Mat applyHistogramEq(const Mat& src) {

    // Convert BGR → YCrCb  (Y = brightness, Cr/Cb = color info)
    Mat ycrcb;
    cvtColor(src, ycrcb, COLOR_BGR2YCrCb);

    // Split into 3 separate channel images
    vector<Mat> channels;
    split(ycrcb, channels);

    // Equalize only the Y (brightness) channel
    equalizeHist(channels[0], channels[0]);

    // Merge channels back and convert back to BGR
    merge(channels, ycrcb);
    Mat dst;
    cvtColor(ycrcb, dst, COLOR_YCrCb2BGR);

    return dst;
}

#endif // EYOBED_HISTOGRAM_EQ_HPP
