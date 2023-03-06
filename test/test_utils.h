//
// Created by Ibai Gorordo on 2023/03/05.
//

#ifndef TEST_TEST_UTILS_H
#define TEST_TEST_UTILS_H

#include "opencv2/opencv.hpp"

#include "GradientCalculator.h"
#include "NormPoint.h"

constexpr auto quant = 2.0;
constexpr auto n_bins = 1024;
constexpr auto ang_thres = 22.5;
constexpr auto density_th = 0.7;
constexpr auto p = ang_thres / 180.0;
constexpr auto scale = 0.8f;
constexpr auto sigma_scale = 0.6f;
constexpr auto sigma = sigma_scale / scale;
constexpr auto log_eps = 0.0;
constexpr auto prec = M_PI * ang_thres / 180.0;
constexpr auto gradient_threshold = 5.2262518595055063; //quant / sin(prec);
constexpr auto bad_value = -1024;

constexpr auto sprec = 3.0;
const auto h =  (int)(ceil(sigma * sqrt(2 * sprec * log(10.0))));
const cv::Size ksize(1 + 2 * h, 1 + 2 * h); // kernel size

cv::Mat gaussian_resize(const cv::Mat& input_img);

void calculate_gradient_opencv(GradientCalculator* gradientCalculator,
                        const cv::Mat& resized_img,
                        cv::Mat& magnitude_img,
                        cv::Mat& ang_img,
                        cv::Mat& bad_pixels_img);

void sort_pixels_opencv(const cv::Mat& magnitude_img,
                        const cv::Mat& bad_pixels_img,
                        std::vector<NormPoint>& orderer_points);

double calculate_max_gradient(const cv::Mat& magnitude_img,
                              const cv::Mat& bad_pixels_img);

cv::Mat draw_map(const cv::Mat& mat, bool use_min=true);

cv::Mat draw_image_diff(cv::Mat& img1,
                        cv::Mat& img2,
                        bool use_min=true,
                        const std::string& title="diff",
                        bool show=false);

cv::Mat draw_lines(cv::Mat& gray_img,
                   const std::vector<std::pair<cv::Point2d, cv::Point2d>>& lines,
                   const std::string& title="lines",
                   bool show=false);

#endif//TEST_TEST_UTILS_H
