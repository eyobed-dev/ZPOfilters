#ifndef SEPIA_FILTER_HPP
#define SEPIA_FILTER_HPP

#include "Filter.hpp"

class SepiaFilter : public Filter {
public:
    std::string getName() const override {
        return "Sepia Tone";
    }

    cv::Mat apply(const cv::Mat& src) override {
        cv::Mat dst;
        cv::Mat kernel = (cv::Mat_<float>(3, 3) << 
            0.272, 0.534, 0.131,
            0.349, 0.686, 0.168,
            0.393, 0.769, 0.189);
        
        cv::transform(src, dst, kernel);
        return dst;
    }
};

#endif // SEPIA_FILTER_HPP
