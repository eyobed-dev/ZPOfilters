#ifndef DENIS_LOWPASS_FFT_HPP
#define DENIS_LOWPASS_FFT_HPP

#include <opencv2/opencv.hpp>
using namespace cv;

// Low-Pass Filter (Frequency Domain) — keeps only the low-frequency
// content of the image, which blurs it.
//
// How it works:
//   1. DFT converts the image from pixels → frequencies.
//   2. A circular mask zeros out all high frequencies (fine detail, edges).
//   3. Inverse DFT converts back → blurred image.
//
// Low frequencies = large shapes and smooth gradients.
// High frequencies = edges, textures, noise.
Mat applyLowPassFFT(const Mat& src) {

    // Work in grayscale (DFT is per-channel; grayscale keeps it simple)
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    gray.convertTo(gray, CV_32F);

    // Step 1 — Forward DFT (complex output: real + imaginary planes)
    Mat complex;
    dft(gray, complex, DFT_COMPLEX_OUTPUT);

    // Step 2 — Shift so the DC component (lowest frequency) is at center.
    //          OpenCV's DFT puts DC at the corners by default.
    int cx = complex.cols / 2;
    int cy = complex.rows / 2;
    Mat q0(complex, Rect(0,  0,  cx, cy));   // top-left
    Mat q1(complex, Rect(cx, 0,  cx, cy));   // top-right
    Mat q2(complex, Rect(0,  cy, cx, cy));   // bottom-left
    Mat q3(complex, Rect(cx, cy, cx, cy));   // bottom-right
    Mat tmp;
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);  // swap TL ↔ BR
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);  // swap TR ↔ BL

    // Step 3 — Build a circular mask centered on DC.
    //          Frequencies inside the circle are kept; outside are zeroed.
    //          Smaller radius = more blur (fewer frequencies kept).
    int r = min(cx, cy) / 4;
    Mat mask = Mat::zeros(complex.size(), CV_32FC2);
    circle(mask, Point(cx, cy), r, Scalar(1, 1), FILLED);
    multiply(complex, mask, complex);

    // Step 4 — Undo the quadrant shift before inverse DFT
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);

    // Step 5 — Inverse DFT → back to spatial domain
    Mat result;
    idft(complex, result, DFT_REAL_OUTPUT | DFT_SCALE);

    // Step 6 — Normalize and convert to 8-bit for display
    normalize(result, result, 0, 255, NORM_MINMAX);
    result.convertTo(result, CV_8U);

    return result;
}

#endif // DENIS_LOWPASS_FFT_HPP
