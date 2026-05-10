#include <filesystem>
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

void redraw() {

  if (src.empty())
    return;

  Mat filtered = allFilters[filterIdx].apply(src.clone());
  imshow("Original", src);
  imshow("Filtered", filtered);

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
  redraw();
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

  namedWindow("Controls", WINDOW_NORMAL);
  resizeWindow("Controls", 400, 60);
  if ((int)imageFiles.size() > 1)
    createTrackbar("Image ", "Controls", NULL, (int)imageFiles.size() - 1,
                   onImageChange);
  if (FILTER_COUNT > 1)
    createTrackbar("Filter", "Controls", NULL, FILTER_COUNT - 1,
                   onFilterChange);
  redraw();
  waitKey(0);
  return 0;
}
