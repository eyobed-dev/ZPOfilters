#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Eyobed
#include "eyobed/histogram_eq.hpp"
#include "eyobed/vignette_blur.hpp"
#include "eyobed/posterize.hpp"
#include "eyobed/lens_distortion.hpp"

// Adar
#include "adar/orton.hpp"
#include "adar/dodge.hpp"
#include "adar/gamma.hpp"
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
    Mat (*apply)(const Mat&);
};

Filter allFilters[] = {
    { "Eyobed", "Histogram Eq",
      "Equalizes image contrast by redistributing\nbrightness. Done in YCrCb space to avoid color shift problem.",
      applyHistogramEq },
    { "Eyobed", "Lens Distortion",
      "Lens distortion effect.\nPixels are mapped using a lens geometric transform.",
      applyLensDistortion },
    { "Eyobed", "Vignette Blur",
      "Softens edges while keeping the center sharp.\nCombines a box blur with a radial distance mask.",
      applyVignetteBlur },
    { "Eyobed", "Posterize",
      "Reduces the number of colors manually.\nMakes the image look like a poster print.",
      applyPosterize },
    { "Adar", "Orton",
      "Blends original with a manually blurred\nscreen layer for a soft glow.",
      applyOrton },
    { "Adar", "Dodge",
      "Applies Color Dodge manually to boost\nhighlights and contrast.",
      applyDodge },
    { "Adar", "Gamma",
      "Applies manual power-law correction\nper channel with explicit clamping.",
      applyGamma },
    { "Adar", "Bloom",
      "Extracts bright pixels, blurs manually,\nand adds glow back.",
      applyBloom },
    { "Denis", "Threshold",
      "Converts image to black and white using\na manual threshold.",
      applyThreshold },
    { "Denis", "Diffuse",
      "Creates frosted-glass effect by manually\nscattering pixels.",
      applyDiffuse },
    { "Denis", "Crosshatch",
      "Sketch-like shading via manual darkness\nbased hatching logic.",
      applyCrosshatch },
    { "Denis", "Hue Rotate",
      "Shifts hue manually in HSV space.",
      applyHueRotate },
};

const int FILTER_COUNT = sizeof(allFilters) / sizeof(allFilters[0]);
const string WINDOW_NAME = "ZPO Filters";

vector<string> imageFiles;
vector<Mat> originalImages;
vector<Mat> galleryThumbs;
vector<pair<Rect, int>> thumbHitboxes;
vector<pair<Rect, int>> filterHitboxes;
vector<Rect> paramHitboxes;
Rect closeButtonRect;

int imageIdx = 0;
int filterIdx = 0;
bool shouldExit = false;

// Unified parameter state (used by filter setters).
double uiGamma = 2.2;
double uiOrtonSigma = 2.0;
double uiOrtonStrength = 0.6;
double uiDodgeStrength = 1.0;
double uiDodgeProtect = 0.0;
double uiBloomRadius = 3.0;
double uiBloomThreshold = 180.0;
double uiBloomStrength = 1.0;

struct ParamBinding {
    string label;
    double minValue;
    double maxValue;
    double* value;
    bool integerOnly;
};

vector<ParamBinding> currentParams;
int activeParamDrag = -1;

Mat toDisplay(const Mat& src) {
    Mat out = src.clone();
    if (out.channels() == 1) cvtColor(out, out, COLOR_GRAY2BGR);
    if (out.depth() == CV_32F) out.convertTo(out, CV_8UC3, 255.0);
    return out;
}

Mat resizeToFit(const Mat& src, int targetW, int targetH) {
    if (src.empty()) return src;
    double scale = min((double)targetW / src.cols, (double)targetH / src.rows);
    if (scale >= 1.0) return src;
    Mat resized;
    resize(src, resized, Size(), scale, scale, INTER_AREA);
    return resized;
}

string formatParamValue(const ParamBinding& param) {
    ostringstream ss;
    if (param.integerOnly) {
        ss << (int)std::round(*param.value);
    } else {
        ss << fixed << setprecision(2) << *param.value;
    }
    return ss.str();
}

string formatFixed(double value, int precision = 2) {
    ostringstream ss;
    ss << fixed << setprecision(precision) << value;
    return ss.str();
}

void applyParameterState() {
    setGammaValue(uiGamma);
    setOrtonSigma(uiOrtonSigma);
    setOrtonStrength(uiOrtonStrength);
    setDodgeStrength(uiDodgeStrength);
    setDodgeProtect((int)std::round(uiDodgeProtect));
    setBloomRadius((int)std::round(uiBloomRadius));
    setBloomThreshold((int)std::round(uiBloomThreshold));
    setBloomStrength(uiBloomStrength);
}

void updateCurrentParams() {
    currentParams.clear();
    const string& filterName = allFilters[filterIdx].name;
    if (filterName == "Gamma") {
        currentParams.push_back({ "Gamma", 0.05, 5.00, &uiGamma, false });
    } else if (filterName == "Orton") {
        currentParams.push_back({ "Sigma", 0.10, 20.00, &uiOrtonSigma, false });
        currentParams.push_back({ "Strength", 0.00, 1.00, &uiOrtonStrength, false });
    } else if (filterName == "Dodge") {
        currentParams.push_back({ "Strength", 0.00, 2.00, &uiDodgeStrength, false });
        currentParams.push_back({ "Protect", 0.00, 128.00, &uiDodgeProtect, true });
    } else if (filterName == "Bloom") {
        currentParams.push_back({ "Radius", 1.00, 12.00, &uiBloomRadius, true });
        currentParams.push_back({ "Threshold", 0.00, 255.00, &uiBloomThreshold, true });
        currentParams.push_back({ "Glow", 0.00, 3.00, &uiBloomStrength, false });
    }
}

void onImageChange(int pos, void*) {
    imageIdx = max(0, min((int)originalImages.size() - 1, pos));
}

void onFilterChange(int pos, void*) {
    filterIdx = max(0, min(FILTER_COUNT - 1, pos));
    updateCurrentParams();
}

void updateParamFromMouseX(int paramIndex, int mouseX) {
    if (paramIndex < 0 || paramIndex >= (int)currentParams.size()) return;
    if (paramIndex >= (int)paramHitboxes.size()) return;

    const Rect& bar = paramHitboxes[paramIndex];
    if (bar.width <= 0) return;

    double t = (double)(mouseX - bar.x) / (double)bar.width;
    t = max(0.0, min(1.0, t));

    ParamBinding& binding = currentParams[paramIndex];
    double value = binding.minValue + t * (binding.maxValue - binding.minValue);
    if (binding.integerOnly) value = std::round(value);
    *binding.value = value;
    applyParameterState();
}

void onMouse(int event, int x, int y, int flags, void*) {
    if (event == EVENT_LBUTTONDOWN) {
        if (closeButtonRect.contains(Point(x, y))) {
            shouldExit = true;
            return;
        }

        for (const auto& hit : thumbHitboxes) {
            if (hit.first.contains(Point(x, y))) {
                imageIdx = hit.second;
                return;
            }
        }

        for (const auto& hit : filterHitboxes) {
            if (hit.first.contains(Point(x, y))) {
                filterIdx = hit.second;
                updateCurrentParams();
                return;
            }
        }

        for (int i = 0; i < (int)paramHitboxes.size(); ++i) {
            if (paramHitboxes[i].contains(Point(x, y))) {
                activeParamDrag = i;
                updateParamFromMouseX(i, x);
                return;
            }
        }
    } else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON)) {
        if (activeParamDrag != -1) updateParamFromMouseX(activeParamDrag, x);
    } else if (event == EVENT_LBUTTONUP) {
        activeParamDrag = -1;
    }
}

void drawMultilineText(Mat& canvas, const string& text, Point origin, int lineStep, Scalar color, double scale = 0.45) {
    string line;
    int y = origin.y;
    for (char ch : text) {
        if (ch == '\n') {
            putText(canvas, line, Point(origin.x, y), FONT_HERSHEY_SIMPLEX, scale, color, 1, LINE_AA);
            y += lineStep;
            line.clear();
        } else {
            line += ch;
        }
    }
    if (!line.empty()) {
        putText(canvas, line, Point(origin.x, y), FONT_HERSHEY_SIMPLEX, scale, color, 1, LINE_AA);
    }
}

void redraw() {
    if (originalImages.empty()) return;

    applyParameterState();
    Mat filtered = toDisplay(allFilters[filterIdx].apply(originalImages[imageIdx].clone()));

    const int canvasW = 1320;
    const int canvasH = 860;
    const int leftPanelW = 360;
    const int galleryH = 170;
    const int margin = 12;

    Rect infoRect(margin, margin, leftPanelW - 2 * margin, canvasH - galleryH - 2 * margin);
    Rect previewRect(leftPanelW + margin, margin, canvasW - leftPanelW - 2 * margin, canvasH - galleryH - 2 * margin);
    Rect galleryRect(margin, canvasH - galleryH + 8, canvasW - 2 * margin, galleryH - 20);
    closeButtonRect = Rect(canvasW - 40, 10, 24, 24);

    Mat canvas(canvasH, canvasW, CV_8UC3, Scalar(28, 28, 28));
    rectangle(canvas, infoRect, Scalar(60, 60, 60), 1);
    rectangle(canvas, previewRect, Scalar(60, 60, 60), 1);
    rectangle(canvas, galleryRect, Scalar(60, 60, 60), 1);

    const Filter& activeFilter = allFilters[filterIdx];
    putText(canvas, "Image and Filter Browser", Point(infoRect.x + 10, infoRect.y + 24),
            FONT_HERSHEY_SIMPLEX, 0.6, Scalar(90, 220, 255), 1, LINE_AA);

    // Prominent filter badge to make selection obvious while using the trackbar.
    Rect selectedBadge(infoRect.x + 8, infoRect.y + 32, infoRect.width - 16, 24);
    rectangle(canvas, selectedBadge, Scalar(70, 70, 120), FILLED);
    rectangle(canvas, selectedBadge, Scalar(150, 180, 255), 1);
    string selectedLine = "Current Filter [" + to_string(filterIdx + 1) + "/" + to_string(FILTER_COUNT) +
                          "]: " + activeFilter.author + " - " + activeFilter.name;
    putText(canvas, selectedLine, Point(selectedBadge.x + 6, selectedBadge.y + 17),
            FONT_HERSHEY_SIMPLEX, 0.46, Scalar(240, 245, 255), 1, LINE_AA);

    int sectionY = infoRect.y + 68;
    putText(canvas, "All filters:", Point(infoRect.x + 10, sectionY),
            FONT_HERSHEY_SIMPLEX, 0.47, Scalar(170, 220, 255), 1, LINE_AA);
    sectionY += 8;

    const int rowH = 16;
    filterHitboxes.clear();
    for (int i = 0; i < FILTER_COUNT; ++i) {
        Rect rowRect(infoRect.x + 8, sectionY, infoRect.width - 16, rowH);
        if (i == filterIdx) {
            rectangle(canvas, rowRect, Scalar(68, 92, 145), FILLED);
            rectangle(canvas, rowRect, Scalar(130, 170, 255), 1);
        }

        string rowText = to_string(i + 1) + ". " + allFilters[i].author + " - " + allFilters[i].name;
        putText(canvas, rowText, Point(rowRect.x + 4, rowRect.y + 12), FONT_HERSHEY_SIMPLEX,
                0.38, (i == filterIdx) ? Scalar(245, 245, 255) : Scalar(195, 195, 195), 1, LINE_AA);
        filterHitboxes.push_back({ rowRect, i });
        sectionY += rowH + 2;
    }

    sectionY += 8;
    putText(canvas, activeFilter.name + " (" + activeFilter.author + ")",
            Point(infoRect.x + 10, sectionY), FONT_HERSHEY_SIMPLEX, 0.54,
            Scalar(230, 230, 230), 1, LINE_AA);

    sectionY += 26;
    drawMultilineText(canvas, activeFilter.description, Point(infoRect.x + 10, sectionY),
                      20, Scalar(180, 180, 180), 0.45);

    updateCurrentParams();
    paramHitboxes.clear();
    int paramTop = sectionY + 84;
    if (!currentParams.empty()) {
        putText(canvas, "Parameters (drag slider bars):", Point(infoRect.x + 10, paramTop),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(120, 220, 120), 1, LINE_AA);
        paramTop += 24;
        for (const ParamBinding& param : currentParams) {
            string line = param.label + ": " + formatParamValue(param);
            putText(canvas, line, Point(infoRect.x + 10, paramTop), FONT_HERSHEY_SIMPLEX,
                    0.48, Scalar(220, 220, 220), 1, LINE_AA);

            Rect bar(infoRect.x + 10, paramTop + 8, infoRect.width - 30, 14);
            rectangle(canvas, bar, Scalar(70, 70, 70), FILLED);
            rectangle(canvas, bar, Scalar(150, 150, 150), 1);

            double ratio = (*param.value - param.minValue) / (param.maxValue - param.minValue);
            ratio = max(0.0, min(1.0, ratio));
            Rect fill = bar;
            fill.width = (int)std::round(ratio * bar.width);
            rectangle(canvas, fill, Scalar(80, 190, 255), FILLED);
            rectangle(canvas, bar, Scalar(180, 180, 180), 1);

            paramHitboxes.push_back(bar);
            paramTop += 44;
        }
    }

    // Extra lightweight per-filter guidance (enabled for Adar filters).
    if (activeFilter.author == "Adar") {
        int extraTop = paramTop + 10;
        putText(canvas, "Channel means:", Point(infoRect.x + 10, extraTop),
                FONT_HERSHEY_SIMPLEX, 0.47, Scalar(120, 220, 120), 1, LINE_AA);
        extraTop += 18;

        Scalar m = mean(filtered);
        int meanB = (int)std::round(m[0]);
        int meanG = (int)std::round(m[1]);
        int meanR = (int)std::round(m[2]);
        putText(canvas, "BGR: " + to_string(meanB) + ", " + to_string(meanG) + ", " + to_string(meanR),
                Point(infoRect.x + 10, extraTop), FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
        extraTop += 18;
        putText(canvas, "RGB: " + to_string(meanR) + ", " + to_string(meanG) + ", " + to_string(meanB),
                Point(infoRect.x + 10, extraTop), FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
        extraTop += 22;

        if (activeFilter.name == "Orton") {
            putText(canvas, "Kernel preview (3x3 Gaussian):", Point(infoRect.x + 10, extraTop),
                    FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
            extraTop += 16;

            double sigma = std::max(0.10, uiOrtonSigma);
            double k[3][3];
            double sum = 0.0;
            for (int j = -1; j <= 1; ++j) {
                for (int i = -1; i <= 1; ++i) {
                    double w = std::exp(-(i * i + j * j) / (2.0 * sigma * sigma));
                    k[j + 1][i + 1] = w;
                    sum += w;
                }
            }
            for (int r = 0; r < 3; ++r) {
                string row = "[" + formatFixed(k[r][0] / sum, 3) + "  " +
                             formatFixed(k[r][1] / sum, 3) + "  " +
                             formatFixed(k[r][2] / sum, 3) + "]";
                putText(canvas, row, Point(infoRect.x + 18, extraTop),
                        FONT_HERSHEY_SIMPLEX, 0.42, Scalar(185, 185, 185), 1, LINE_AA);
                extraTop += 16;
            }
        } else if (activeFilter.name == "Bloom") {
            int r = (int)std::round(uiBloomRadius);
            int ksize = 2 * r + 1;
            int area = std::max(1, ksize * ksize);
            putText(canvas, "Kernel: box " + to_string(ksize) + "x" + to_string(ksize) +
                             ", each weight ~1/" + to_string(area),
                    Point(infoRect.x + 10, extraTop), FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
            extraTop += 18;
            putText(canvas, "Model: output = input + glow * strength",
                    Point(infoRect.x + 10, extraTop), FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
        } else if (activeFilter.name == "Gamma") {
            putText(canvas, "LUT curve: I' = 255*(I/255)^(1/gamma)",
                    Point(infoRect.x + 10, extraTop), FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
            extraTop += 18;
            putText(canvas, "LUT size: 256 entries",
                    Point(infoRect.x + 10, extraTop), FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
        } else if (activeFilter.name == "Dodge") {
            putText(canvas, "Curve: C = 255*I/(255-I+protect)",
                    Point(infoRect.x + 10, extraTop), FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
            extraTop += 18;
            putText(canvas, "Output = mix(input, C, strength)",
                    Point(infoRect.x + 10, extraTop), FONT_HERSHEY_SIMPLEX, 0.44, Scalar(205, 205, 205), 1, LINE_AA);
        }
    }

    rectangle(canvas, closeButtonRect, Scalar(30, 30, 180), FILLED);
    rectangle(canvas, closeButtonRect, Scalar(220, 220, 220), 1);
    putText(canvas, "X", Point(closeButtonRect.x + 7, closeButtonRect.y + 17),
            FONT_HERSHEY_SIMPLEX, 0.55, Scalar(255, 255, 255), 1, LINE_AA);

    Mat previewFit = resizeToFit(filtered, previewRect.width - 8, previewRect.height - 8);
    int px = previewRect.x + (previewRect.width - previewFit.cols) / 2;
    int py = previewRect.y + (previewRect.height - previewFit.rows) / 2;
    previewFit.copyTo(canvas(Rect(px, py, previewFit.cols, previewFit.rows)));

    putText(canvas, "Click a thumbnail to pick image", Point(galleryRect.x + 8, galleryRect.y + 18),
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 200, 200), 1, LINE_AA);

    thumbHitboxes.clear();
    const int thumbW = 120;
    const int thumbH = 85;
    const int gap = 10;
    const int maxVisible = max(1, (galleryRect.width - 20) / (thumbW + gap));

    int start = max(0, imageIdx - maxVisible / 2);
    if (start + maxVisible > (int)galleryThumbs.size()) start = max(0, (int)galleryThumbs.size() - maxVisible);

    int x = galleryRect.x + 10;
    int y = galleryRect.y + 30;
    for (int i = start; i < (int)galleryThumbs.size() && i < start + maxVisible; ++i) {
        Rect thumbRect(x, y, thumbW, thumbH);
        Mat thumb = galleryThumbs[i];
        if (!thumb.empty()) {
            thumb.copyTo(canvas(thumbRect));
        }
        Scalar border = (i == imageIdx) ? Scalar(80, 220, 255) : Scalar(130, 130, 130);
        int thickness = (i == imageIdx) ? 2 : 1;
        rectangle(canvas, thumbRect, border, thickness);

        string idxLabel = to_string(i + 1);
        putText(canvas, idxLabel, Point(thumbRect.x + 4, thumbRect.y + thumbRect.height - 6),
                FONT_HERSHEY_SIMPLEX, 0.42, Scalar(230, 230, 230), 1, LINE_AA);
        thumbHitboxes.push_back({ thumbRect, i });
        x += thumbW + gap;
    }

    imshow(WINDOW_NAME, canvas);
}

int main(int argc, char* argv[]) {
    string dataDir = "../data";
    for (const auto& entry : filesystem::directory_iterator(dataDir)) {
        if (!entry.is_regular_file()) continue;
        string extension = entry.path().extension().string();
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" || extension == ".bmp") {
            imageFiles.push_back(entry.path().string());
        }
    }
    sort(imageFiles.begin(), imageFiles.end());

    for (const string& path : imageFiles) {
        Mat img = imread(path);
        if (img.empty()) continue;
        originalImages.push_back(img);
    }

    if (originalImages.empty()) {
        cerr << "No images found in ../data. Add jpg/png/bmp images and rerun." << endl;
        return 1;
    }

    for (const Mat& img : originalImages) {
        Mat thumb;
        resize(img, thumb, Size(120, 85), 0, 0, INTER_AREA);
        galleryThumbs.push_back(thumb);
    }

    applyParameterState();
    updateCurrentParams();

    namedWindow(WINDOW_NAME, WINDOW_NORMAL);
    resizeWindow(WINDOW_NAME, 1320, 860);
    setMouseCallback(WINDOW_NAME, onMouse);

    while (true) {
        redraw();
        int key = waitKey(20);
        if (key == 27 || key == 'q' || key == 'Q' || shouldExit) break;
    }

    return 0;
}
