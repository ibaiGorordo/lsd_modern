//
// Created by ibaig on 1/3/2023.
//
#include <memory>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "GradientCalculator.h"
#include "RegionDetector.h"


std::unique_ptr<GradientCalculator> gradientCalculator;
std::unique_ptr<RegionDetector> regionDetector;

cv::Mat gradx_img, grady_img, magnitude_img, bad_pixels_img;

cv::Mat draw_map(const cv::Mat& mat, bool use_min=true)
{
    cv::Mat adjMap;
    double min, max;
    cv::minMaxLoc(mat, &min, &max);
    if (!use_min) min = 0;
    mat.convertTo(adjMap, CV_8UC1, 255 / (max-min), -min);
    cv::applyColorMap(adjMap, adjMap, cv::COLORMAP_JET);
    return adjMap;
}

cv::Mat gaussian_resize(const cv::Mat& input_img,
                        int ksize = 7,
                        double sigma = 0.6/0.8,
                        double scale = 0.8)
{
    cv::Mat resized_img;
    cv::GaussianBlur(input_img, resized_img,
                     cv::Size(ksize,ksize), sigma);

    cv::Size new_size(int(ceil(input_img.cols * scale)),
                      int(ceil(input_img.rows * scale)));
    resize(resized_img, resized_img, new_size, 0, 0, cv::INTER_LINEAR_EXACT);

    return resized_img;
}

void calculate_gradient(cv::Mat resized_img)
{
    auto threshold = 5.2262518595055063;
    if(gradientCalculator == nullptr)
    {
        gradientCalculator = std::make_unique<GradientCalculator>(threshold);
    }

    gradx_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    grady_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    magnitude_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    bad_pixels_img = cv::Mat(resized_img.rows, resized_img.cols, CV_8U);

    auto *imagePtr = reinterpret_cast<unsigned char *>(resized_img.data);
    auto *grad_xPtr = reinterpret_cast<double *>(gradx_img.data);
    auto *grad_yPtr = reinterpret_cast<double *>(grady_img.data);
    auto *magnitudesPtr = reinterpret_cast<double *>(magnitude_img.data);
    auto *bad_pixelsPtr = reinterpret_cast<unsigned char *>(bad_pixels_img.data);


    gradientCalculator->calculateGradients(imagePtr,
                                           resized_img.cols, resized_img.rows,
                                           grad_xPtr, grad_yPtr,
                                           magnitudesPtr, bad_pixelsPtr);
}

int main() {

    cv::Mat gray_img = cv::imread("assets/bathroom.jpg",
                                  cv::IMREAD_GRAYSCALE);
    auto resized_img = gaussian_resize(gray_img);

    auto ang_thres = 22.5;
    regionDetector = std::make_unique<RegionDetector>(ang_thres);

    auto num_test = 100;
    long long int total_time = 0;
    for(int i = 0; i < num_test; i++)
    {
        calculate_gradient(resized_img);

        auto start = std::chrono::high_resolution_clock::now();
        regionDetector->detect(gradx_img.ptr<double>(),
                               grady_img.ptr<double>(),
                               magnitude_img.ptr<double>(),
                               bad_pixels_img.ptr<unsigned char>(),
                               resized_img.cols, resized_img.rows);
        auto stop = std::chrono::high_resolution_clock::now();
        total_time+= std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    }

    printf("Time taken by function: %lld microseconds", total_time/num_test);

    auto magnitude_color = draw_map(magnitude_img);
    cv::namedWindow("magnitude", cv::WINDOW_NORMAL);
    cv::imshow("magnitude", magnitude_color);
    cv::waitKey(0);

    return 0;
}
