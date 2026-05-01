
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//  STEP 1 — Include your filter hpp file here

// Eyobed
#include "eyobed/histogram_eq.hpp"
#include "eyobed/vignette.hpp"
#include "eyobed/sobel.hpp"

// Adar
#include "adar/brightness.hpp"
#include "adar/median.hpp"

// Denis
#include "denis/lowpass_fft.hpp"
#include "denis/noise.hpp"


struct Filter {
  string name;
  string description;
  Mat (*apply)(const Mat &);
};

//  STEP 2 — Register your filter here

Filter allFilters[] = {

    //  Eyobed
    { "Sobel Edge",
      "Detects edges using the Sobel gradient operator.\nComputes X and Y gradients separately,\nthen combines them into an edge-strength map.",
      applySobel },

    { "Histogram Eq",
      "Equalizes image contrast by redistributing\nbrightness values across the full 0-255 range.\nWorks in YCrCb space to avoid color shifts.",
      applyHistogramEq },

    { "Vignette",
      "Darkens the edges while keeping the center bright.\nUses a 2D Gaussian surface as a brightness mask\n(center=1.0, edges approach 0.0).",
      applyVignette },

    //  Adar
    { "Brightness",
      "Adds a fixed offset (+60) to every pixel.\nValues above 255 are clamped automatically.\nChange the offset to darken or brighten.",
      applyBrightness },

    { "Median",
      "Replaces each pixel with the median value\nof its 5x5 neighborhood.\nEffectively removes salt-and-pepper noise\nwhile preserving edges.",
      applyMedian },

    //  Denis
    { "Low Pass FFT",
      "Removes high frequencies (edges, texture, noise)\nusing the Discrete Fourier Transform.\nOnly frequencies within a circular radius\naround DC are kept.",
      applyLowPassFFT },

    { "Gaussian Noise",
      "Adds random Gaussian noise (mean=0, stddev=25)\nto simulate camera sensor noise or film grain.\nLarger stddev = stronger noise.",
      applyGaussianNoise },

};

const int FILTER_COUNT = sizeof(allFilters) / sizeof(allFilters[0]);
vector<string> imageFiles;
Mat currentOriginal;
int imageIdx = 0;
int filterIdx = 0;
Mat toDisplay(const Mat& src) {
    Mat out = src.clone();
    if (out.channels() == 1) cvtColor(out, out, COLOR_GRAY2BGR);
    if (out.depth() == CV_32F) out.convertTo(out, CV_8UC3, 255.0);
    return out;
}

void redraw() {
    if (currentOriginal.empty()) return;

    imshow("Original", currentOriginal);

    imshow("Filtered", toDisplay(allFilters[filterIdx].apply(currentOriginal.clone())));

    Mat info = Mat::zeros(300, 400, CV_8UC3);
    putText(info, allFilters[filterIdx].name, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(80, 190, 255), 1, LINE_AA);
    int y = 65;
    string cur;
    for (char ch : allFilters[filterIdx].description) {
        if (ch == '\n') {
            putText(info, cur, Point(10, y), FONT_HERSHEY_SIMPLEX, 0.45, Scalar(170, 170, 170), 1, LINE_AA);
            y += 26; cur = "";
        } else cur += ch;
    }
    if (!cur.empty())
        putText(info, cur, Point(10, y), FONT_HERSHEY_SIMPLEX, 0.45, Scalar(170, 170, 170), 1, LINE_AA);

    imshow("Info", info);
}

void onImageChange(int pos, void*) {
    imageIdx = pos;
    currentOriginal = imread(imageFiles[imageIdx]);
    redraw();
}

void onFilterChange(int pos, void*) {
    filterIdx = pos;
    redraw();
}

int main(int argc, char* argv[]) {

    string dataDir = "../data";

    for (const auto& entry : filesystem::directory_iterator(dataDir)) {
        string extension = entry.path().extension().string();
        if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" || extension == ".bmp")
            imageFiles.push_back(entry.path().string());
    }

    currentOriginal = imread(imageFiles[0]);

    namedWindow("Controls", WINDOW_NORMAL);
    resizeWindow("Controls", 400, 60);
    if ((int)imageFiles.size() > 1)
        createTrackbar("Image ", "Controls", NULL, (int)imageFiles.size() - 1, onImageChange);
    if (FILTER_COUNT > 1)
        createTrackbar("Filter", "Controls", NULL, FILTER_COUNT - 1, onFilterChange);

    redraw();
    waitKey(0);
    return 0;
}
