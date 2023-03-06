//
// Created by Ibai Gorordo on 2023/03/05.
//

#include <opencv2/opencv.hpp>

#include "GradientCalculator.h"
#include "PixelSorter.h"
#include "test_utils.h"

using namespace std::chrono;


std::unique_ptr<GradientCalculator> gradientCalculator;
std::vector<NormPoint> ordered_points_cv, ordered_points_custom;
cv::Ptr<cv::LineSegmentDetector> lsd_cv;
cv::Mat magnitude_img, ang_img, bad_pixels_img, resized_img;
cv::Mat gray_img;


void test(const std::function<void()>& pixel_sorter,
          const std::string& func_name,
          int num_tests)
{
    gray_img = cv::imread("assets/bathroom.jpg",
                          cv::IMREAD_GRAYSCALE);

    resized_img = gaussian_resize(gray_img);
    calculate_gradient_opencv(gradientCalculator.get(),
                              resized_img,
                              magnitude_img,
                              ang_img,
                              bad_pixels_img);

    long long total_time = 0;
    for (int i = 0; i < num_tests; ++i)
    {
        auto start = high_resolution_clock::now();
        pixel_sorter();
        auto end = high_resolution_clock::now();
        total_time += duration_cast<microseconds>(end - start).count();
    }
    auto mean_time = total_time / static_cast<long>(num_tests);
    printf("%s: %lld microseconds per call\n", func_name.c_str(), mean_time);
}

void test_opencv_pixel_sorter()
{
    ordered_points_cv.clear();
    sort_pixels_opencv(magnitude_img,
                       bad_pixels_img,
                       ordered_points_cv);
}

void test_custom_pixel_sorter()
{
    ordered_points_custom.clear();
    PixelSorter::sort(ordered_points_custom,
                     magnitude_img.ptr<double>(),
                     bad_pixels_img.ptr<unsigned char>(),
                     resized_img.cols, resized_img.rows);
}


int main() {

    auto num_test = 1;
    gradientCalculator = std::make_unique<GradientCalculator>(gradient_threshold);

    test(test_custom_pixel_sorter, "custom_pixel_sorter", num_test);
    test(test_opencv_pixel_sorter, "opencv_pixel_sorter", num_test);

    assert(ordered_points_cv.size() == ordered_points_custom.size());

    for(int i = 0; i < ordered_points_cv.size(); ++i)
    {
        assert(ordered_points_cv[i].x == ordered_points_custom[i].x);
        assert(ordered_points_cv[i].y == ordered_points_custom[i].y);
    }


    return 0;
}