//
// Created by Ibai Gorordo on 2023/03/05.
//

#include <opencv2/opencv.hpp>

#include "GradientCalculator.h"
#include "RegionDetector.h"

using namespace std::chrono;


std::unique_ptr<GradientCalculator> gradientCalculator;
std::vector<NormPoint> ordered_points;
cv::Ptr<cv::LineSegmentDetector> lsd_cv;
cv::Mat magnitude_img, ang_img, bad_pixels_img, resized_img;
cv::Mat gray_img;

cv::Mat gaussian_resize(const cv::Mat& input_img,
                        int ksize = 7,
                        double sigma = 0.6/0.8,
                        double scale = 0.8)
{
    cv::Mat res_img;
    cv::GaussianBlur(input_img, res_img,
                     cv::Size(ksize,ksize), sigma);

    cv::Size new_size(int(ceil(input_img.cols * scale)),
                      int(ceil(input_img.rows * scale)));
    resize(res_img, res_img, new_size, 0, 0, cv::INTER_LINEAR_EXACT);

    return res_img;
}


int main() {

    auto num_test = 1;

    test(test_pytlsd_pixel_sorter, "pytlsd_pixel_sorter", num_test);
    test(test_custom_pixel_sorter, "custom_pixel_sorter", num_test);
    test(test_opencv_pixel_sorter, "opencv_pixel_sorter", num_test);


    return 0;
}