
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
#include "eyobed/zoom_blur.hpp"
#include "eyobed/posterize.hpp"
#include "eyobed/lens_distortion.hpp"

// Adar
#include "adar/emboss.hpp"
#include "adar/dodge.hpp"
#include "adar/swirl.hpp"
#include "adar/bloom.hpp"

// Denis
#include "denis/threshold.hpp"
#include "denis/diffuse.hpp"
#include "denis/crosshatch.hpp"
#include "denis/hue_rotate.hpp"


struct Filter {
  string author;
  string name;
  string description;
  Mat (*apply)(const Mat &);
};

//  STEP 2 — Register your filter here

Filter allFilters[] = {

    //  Eyobed
    { "Eyobed", "Histogram Eq",
      "Equalizes image contrast by redistributing\nbrightness values across the full 0-255 range.\nWorks in YCrCb space to avoid color shifts.",
      applyHistogramEq },

    { "Eyobed", "Lens Distortion",
      "Applies a manual barrel distortion effect.\nPixels are mapped using a radial geometric transform.",
      applyLensDistortion },

    { "Eyobed", "Zoom Blur",
      "Radial blur effect that streaks pixels from the edges\ntowards the center. Uses a manual box blur kernel.",
      applyZoomBlur },

    { "Eyobed", "Posterize",
      "Reduces the number of colors in the image manually.\nMakes the image look like a comic or retro art.",
      applyPosterize },

    //  Adar
    { "Adar", "Relief Emboss",
      "Calculates the directional difference between\nneighboring pixels to create a 3D gray relief texture.",
      applyEmboss },

    { "Adar", "Dodge",
      "Applies a Color Dodge blend manually to blow out\nhighlights and dramatically increase contrast.",
      applyDodge },

    { "Adar", "Swirl",
      "Warps the image manually using polar coordinates\nto create a geometric vortex effect at the center.",
      applySwirl },

    { "Adar", "Bloom",
      "Manually extracts bright pixels, applies a nested\nbox blur loop, and adds them back to create a glow.",
      applyBloom },

    //  Denis
    { "Denis", "Threshold",
      "Converts the image to pure black and white\nbased on a manual intensity threshold (127).\nImplemented pixel-by-pixel.",
      applyThreshold },

    { "Denis", "Diffuse",
      "Creates a frosted glass effect by manually\nscattering pixels within a small random radius.",
      applyDiffuse },

    { "Denis", "Crosshatch",
      "Applies a sketch-like shading effect.\nDraws diagonal lines whose density depends\non the manual calculation of pixel darkness.",
      applyCrosshatch },

    { "Denis", "Hue Rotate",
      "Shifts the hue of every pixel manually by +60\ndegrees using HSV color space.",
      applyHueRotate },

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

Mat resizeForDisplay(const Mat& src, int maxWidth = 800, int maxHeight = 600) {
    if (src.empty()) return src;
    double scale = min((double)maxWidth / src.cols, (double)maxHeight / src.rows);
    if (scale >= 1.0) return src; 
    Mat resized;
    resize(src, resized, Size(), scale, scale, INTER_AREA);
    return resized;
}

void redraw() {
    if (currentOriginal.empty()) return;

    imshow("Original", resizeForDisplay(currentOriginal));

    imshow("Filtered", resizeForDisplay(toDisplay(allFilters[filterIdx].apply(currentOriginal.clone()))));

    Mat info = Mat::zeros(300, 400, CV_8UC3);
    
    int descY = 30;
    putText(info, allFilters[filterIdx].name + " (by " + allFilters[filterIdx].author + ")", Point(10, descY), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(80, 190, 255), 1, LINE_AA);
    descY += 25;
    
    string cur;
    for (char ch : allFilters[filterIdx].description) {
        if (ch == '\n') {
            putText(info, cur, Point(10, descY), FONT_HERSHEY_SIMPLEX, 0.45, Scalar(200, 200, 200), 1, LINE_AA);
            descY += 22; cur = "";
        } else cur += ch;
    }
    if (!cur.empty())
        putText(info, cur, Point(10, descY), FONT_HERSHEY_SIMPLEX, 0.45, Scalar(200, 200, 200), 1, LINE_AA);

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
