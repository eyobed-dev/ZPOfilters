#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

/**
 * ZPOe Project: Filter Effects
 * 
 */

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1;
    }

    std::string imagePath = argv[1];
    cv::Mat src = cv::imread(imagePath);

    if (src.empty()) {
        std::cerr << "Could not open image" << std::endl;
        return -1;
    }

    //  register  filters here
    std::vector<std::shared_ptr<Filter>> filters;
    filters.push_back(std::make_shared<SepiaFilter>());

    std::cout << "select Filter:" << std::endl;
    for (size_t i = 0; i < filters.size(); ++i) {
        std::cout << i << ": " << filters[i]->getName() << std::endl;
    }

    int choice = 0;
    if (filters.size() > 1) {
        std::cout << "Choose a filter number: ";
        std::cin >> choice;
    }

    if (choice >= 0 && (size_t)choice < filters.size()) {
        cv::Mat dst = filters[choice]->apply(src);
        
        cv::imshow("Original", src);
        cv::imshow(filters[choice]->getName(), dst);
        
        std::cout << "Press any key to exit..." << std::endl;
        cv::waitKey(0);
    } else {
        std::cout << "wrongg choice." << std::endl;
    }

    return 0;
}
