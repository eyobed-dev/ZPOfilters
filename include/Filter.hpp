#ifndef FILTER_HPP
#define FILTER_HPP

#include <opencv2/opencv.hpp>
#include <string>

/**
 * Filter Abstract Base Class
 * 
 * Each should implement your own filters by inheriting
 * from this class and overriding the apply() method.
 */
class Filter {
public:
    virtual ~Filter() {}

    /**
     * Apply the filter to the source image.
     * @param src The source image (OpenCV Mat).
     * @return The filtered image.
     */
    virtual cv::Mat apply(const cv::Mat& src) = 0;

    /**
     * Get the name of the filter.
     * @return String containing the filter name.
     */
    virtual std::string getName() const = 0;
};

#endif // FILTER_HPP
