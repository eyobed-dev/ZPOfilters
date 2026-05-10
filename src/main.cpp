#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Including filter hpp files

// Eyobed
#include "eyobed/histogram_eq.hpp"
#include "eyobed/lens_distortion.hpp"
#include "eyobed/posterize.hpp"
#include "eyobed/vignette_blur.hpp"

// Adar
#include "adar/bloom.hpp"
#include "adar/dodge.hpp"
#include "adar/gamma.hpp"
#include "adar/orton.hpp"

// Denis
#include "denis/crosshatch.hpp"
#include "denis/diffuse.hpp"
#include "denis/hue_rotate.hpp"
#include "denis/threshold.hpp"

struct Filter {
  string author;
  string name;
  string description;
  // stores the address of a filter function that takes a Mat and returns a Mat
  // unchanged
  Mat (*apply)(const Mat &);
};

// Register filters

Filter allFilters[] = {

    // Eyobed
    {"Eyobed", "Histogram Eq",
     "Equalizes image contrast by redistributing\nbrightness. Done in YCrCb "
     "space to avoid color shift problem.",
     applyHistogramEq},

    {"Eyobed", "Lens Distortion",
     "lens distortion effect.\nPixels are mapped using a lens geometric "
     "transform.",
     applyLensDistortion},

    {"Eyobed", "Vignette Blur",
     "Softens the edges of the image while keeping\nthe center "
     "sharp.\nCombines a 15x15 box blur\nwith a radial distanc mask.",
     applyVignetteBlur},

    {"Eyobed", "Posterize",
     "Reduces the number of colors in the image manually.\nMakes the image "
     "look like a poster print.",
     applyPosterize},

    // Adar
    {"Adar", "Orton",
     "Blends the original with a manually blurred screen\nlayer to create a "
     "soft cinematic glow.",
     applyOrton},

    {"Adar", "Dodge",
     "Applies a Color Dodge blend manually to blow out\nhighlights and "
     "dramatically increase contrast.",
     applyDodge},

    {"Adar", "Gamma",
     "Applies manual power-law correction per channel\nwith pixel-wise math "
     "and explicit clamping.",
     applyGamma},

    {"Adar", "Bloom",
     "Manually extracts bright pixels, applies a nested\nbox blur loop, and "
     "adds them back to create a glow.",
     applyBloom},

    // Denis
    {"Denis", "Threshold",
     "Converts the image to pure black and white\nbased on a manual intensity "
     "threshold (127).\nImplemented pixel-by-pixel.",
     applyThreshold},

    {"Denis", "Diffuse",
     "Creates a frosted glass effect by manually\nscattering pixels within a "
     "small random radius.",
     applyDiffuse},

    {"Denis", "Crosshatch",
     "Applies a sketch-like shading effect.\nDraws diagonal lines whose "
     "density depends\non the manual calculation of pixel darkness.",
     applyCrosshatch},

    {"Denis", "Hue Rotate",
     "Shifts the hue of every pixel manually by +60\ndegrees using HSV color "
     "space.",
     applyHueRotate},

};

const int FILTER_COUNT = sizeof(allFilters) / sizeof(allFilters[0]);
vector<string> imageFiles;
Mat src;
int imageIdx = 0;
int filterIdx = 0;

int gammaSlider = 220;          // 2.20
int ortonSigmaSlider = 200;     // 2.00
int ortonStrengthSlider = 60;   // 0.60
int dodgeStrengthSlider = 100;  // 1.00
int dodgeProtectSlider = 0;     // 0
int bloomRadiusSlider = 3;      // 3
int bloomThresholdSlider = 180; // 180
int bloomStrengthSlider = 100;  // 1.00

constexpr int GAMMA_MIN_SLIDER = 5;            // 0.05
constexpr int GAMMA_MAX_SLIDER = 500;          // 5.00
constexpr int ORTON_SIGMA_MIN_SLIDER = 10;     // 0.10
constexpr int ORTON_SIGMA_MAX_SLIDER = 2000;   // 20.00
constexpr int DODGE_STRENGTH_MAX_SLIDER = 200; // 2.00
constexpr int DODGE_PROTECT_MAX_SLIDER = 128;
constexpr int BLOOM_RADIUS_MAX_SLIDER = 12;
constexpr int BLOOM_THRESHOLD_MAX_SLIDER = 255;
constexpr int BLOOM_STRENGTH_MAX_SLIDER = 300; // 3.00

const string PARAM_WINDOW = "Filter Parameters";
bool isParamWindowVisible = false;

void onGammaChange(int pos, void *);
void onOrtonSigmaChange(int pos, void *);
void onOrtonStrengthChange(int pos, void *);
void onDodgeStrengthChange(int pos, void *);
void onDodgeProtectChange(int pos, void *);
void onBloomRadiusChange(int pos, void *);
void onBloomThresholdChange(int pos, void *);
void onBloomStrengthChange(int pos, void *);
void updateParameterWindow();

void redraw() {
  if (src.empty())
    return;
  imshow("Original", src);
  imshow("Filtered", allFilters[filterIdx].apply(
                         src.clone())); // apply cloned src to fitered

  Mat info = Mat::zeros(300, 400, CV_8UC3); // Display filter info
  int descY = 30;
  putText(info,
          allFilters[filterIdx].name + " (by " + allFilters[filterIdx].author +
              ")",
          Point(10, descY), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(80, 190, 255), 1,
          LINE_AA);
  descY += 25;

  string cur;
  for (char ch : allFilters[filterIdx].description) {
    if (ch == '\n') {
      putText(info, cur, Point(10, descY), FONT_HERSHEY_SIMPLEX, 0.45,
              Scalar(200, 200, 200), 1, LINE_AA);
      descY += 22;
      cur = "";
    } else
      cur += ch;
  }
  if (!cur.empty())
    putText(info, cur, Point(10, descY), FONT_HERSHEY_SIMPLEX, 0.45,
            Scalar(200, 200, 200), 1, LINE_AA);

  imshow("Info", info);
}

void onImageChange(int pos, void *) {
  imageIdx = pos;
  src = imread(imageFiles[imageIdx]);
  redraw();
}

void onFilterChange(int pos, void *) {
  filterIdx = pos;
  updateParameterWindow();
  redraw();
}

void onGammaChange(int pos, void *) {
  int safePos = max(GAMMA_MIN_SLIDER, pos);
  if (safePos != pos) {
    gammaSlider = safePos;
    if (isParamWindowVisible)
      setTrackbarPos("Gamma x0.01", PARAM_WINDOW, safePos);
  }

  setGammaValue(safePos / 100.0);
  redraw();
}

void onOrtonSigmaChange(int pos, void *) {
  int safePos = max(ORTON_SIGMA_MIN_SLIDER, pos);
  if (safePos != pos) {
    ortonSigmaSlider = safePos;
    if (isParamWindowVisible)
      setTrackbarPos("Sigma x0.01", PARAM_WINDOW, safePos);
  }
  setOrtonSigma(safePos / 100.0);
  redraw();
}

void onOrtonStrengthChange(int pos, void *) {
  ortonStrengthSlider = max(0, min(100, pos));
  if (ortonStrengthSlider != pos && isParamWindowVisible)
    setTrackbarPos("Strength %", PARAM_WINDOW, ortonStrengthSlider);
  setOrtonStrength(ortonStrengthSlider / 100.0);
  redraw();
}

void onDodgeStrengthChange(int pos, void *) {
  dodgeStrengthSlider = max(0, min(DODGE_STRENGTH_MAX_SLIDER, pos));
  if (dodgeStrengthSlider != pos && isParamWindowVisible)
    setTrackbarPos("Strength x0.01", PARAM_WINDOW, dodgeStrengthSlider);
  setDodgeStrength(dodgeStrengthSlider / 100.0);
  redraw();
}

void onDodgeProtectChange(int pos, void *) {
  dodgeProtectSlider = max(0, min(DODGE_PROTECT_MAX_SLIDER, pos));
  if (dodgeProtectSlider != pos && isParamWindowVisible)
    setTrackbarPos("Protect", PARAM_WINDOW, dodgeProtectSlider);
  setDodgeProtect(dodgeProtectSlider);
  redraw();
}

void onBloomRadiusChange(int pos, void *) {
  bloomRadiusSlider = max(1, min(BLOOM_RADIUS_MAX_SLIDER, pos));
  if (bloomRadiusSlider != pos && isParamWindowVisible)
    setTrackbarPos("Radius", PARAM_WINDOW, bloomRadiusSlider);
  setBloomRadius(bloomRadiusSlider);
  redraw();
}

void onBloomThresholdChange(int pos, void *) {
  bloomThresholdSlider = max(0, min(BLOOM_THRESHOLD_MAX_SLIDER, pos));
  if (bloomThresholdSlider != pos && isParamWindowVisible)
    setTrackbarPos("Threshold", PARAM_WINDOW, bloomThresholdSlider);
  setBloomThreshold(bloomThresholdSlider);
  redraw();
}

void onBloomStrengthChange(int pos, void *) {
  bloomStrengthSlider = max(0, min(BLOOM_STRENGTH_MAX_SLIDER, pos));
  if (bloomStrengthSlider != pos && isParamWindowVisible)
    setTrackbarPos("Glow x0.01", PARAM_WINDOW, bloomStrengthSlider);
  setBloomStrength(bloomStrengthSlider / 100.0);
  redraw();
}

void updateParameterWindow() {
  if (isParamWindowVisible) {
    destroyWindow(PARAM_WINDOW);
    isParamWindowVisible = false;
  }

  const string &filterName = allFilters[filterIdx].name;
  if (filterName != "Gamma" && filterName != "Orton" && filterName != "Dodge" &&
      filterName != "Bloom")
    return;

  namedWindow(PARAM_WINDOW, WINDOW_NORMAL);
  resizeWindow(PARAM_WINDOW, 380, 160);
  isParamWindowVisible = true;

  if (filterName == "Gamma") {
    createTrackbar("Gamma x0.01", PARAM_WINDOW, &gammaSlider, GAMMA_MAX_SLIDER,
                   onGammaChange);
  } else if (filterName == "Orton") {
    createTrackbar("Sigma x0.01", PARAM_WINDOW, &ortonSigmaSlider,
                   ORTON_SIGMA_MAX_SLIDER, onOrtonSigmaChange);
    createTrackbar("Strength %", PARAM_WINDOW, &ortonStrengthSlider, 100,
                   onOrtonStrengthChange);
  } else if (filterName == "Dodge") {
    createTrackbar("Strength x0.01", PARAM_WINDOW, &dodgeStrengthSlider,
                   DODGE_STRENGTH_MAX_SLIDER, onDodgeStrengthChange);
    createTrackbar("Protect", PARAM_WINDOW, &dodgeProtectSlider,
                   DODGE_PROTECT_MAX_SLIDER, onDodgeProtectChange);
  } else if (filterName == "Bloom") {
    createTrackbar("Radius", PARAM_WINDOW, &bloomRadiusSlider,
                   BLOOM_RADIUS_MAX_SLIDER, onBloomRadiusChange);
    createTrackbar("Threshold", PARAM_WINDOW, &bloomThresholdSlider,
                   BLOOM_THRESHOLD_MAX_SLIDER, onBloomThresholdChange);
    createTrackbar("Glow x0.01", PARAM_WINDOW, &bloomStrengthSlider,
                   BLOOM_STRENGTH_MAX_SLIDER, onBloomStrengthChange);
  }
}

int main(int argc, char *argv[]) {

  // make sure the faces are in /data
  string dataDir = "../data";

  for (const auto &entry : filesystem::directory_iterator(dataDir)) {
    string extension = entry.path().extension().string();
    if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" ||
        extension == ".bmp")
      imageFiles.push_back(entry.path().string());
  }

  // Sort to order the faces
  sort(imageFiles.begin(), imageFiles.end());

  src = imread(imageFiles[0]);

  setGammaValue(gammaSlider / 100.0);
  setOrtonSigma(ortonSigmaSlider / 100.0);
  setOrtonStrength(ortonStrengthSlider / 100.0);
  setDodgeStrength(dodgeStrengthSlider / 100.0);
  setDodgeProtect(dodgeProtectSlider);
  setBloomRadius(bloomRadiusSlider);
  setBloomThreshold(bloomThresholdSlider);
  setBloomStrength(bloomStrengthSlider / 100.0);

  namedWindow("Controls", WINDOW_NORMAL);
  resizeWindow("Controls", 400, 60);
  if ((int)imageFiles.size() > 1)
    createTrackbar("Image ", "Controls", NULL, (int)imageFiles.size() - 1,
                   onImageChange);
  if (FILTER_COUNT > 1)
    createTrackbar("Filter", "Controls", NULL, FILTER_COUNT - 1,
                   onFilterChange);
  updateParameterWindow();

  redraw();
  waitKey(0);
  return 0;
}
