//
// Created by ibaig on 1/1/2023.
//
#include "chrono"
#include <opencv2/opencv.hpp>

#include "lsd.h"
#include "GradientCalculator.h"

using namespace std::chrono;

std::unique_ptr<GradientCalculator> gradientCalculator;
cv::Mat grad_x, grad_y, bad_pixels;
image_double angles, modgrad;

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

cv::Mat draw_image_diff(const cv::Mat& img1,
                        const cv::Mat& img2,
                        const std::string& title="diff",
                        bool show=false)
{
    cv::Mat diff;
    cv::absdiff(img1, img2, diff);

    // Apply the colormap
    cv::Mat diff_color = draw_map(diff);

    if(show)
    {
        cv::namedWindow(title, cv::WINDOW_NORMAL);
        cv::imshow(title, diff_color);
    }
    return diff_color;
}

cv::Mat test(const std::function<void(cv::Mat&, cv::Mat&, cv::Mat&)>& angle_gradient,
             const std::string& func_name,
             int num_tests,
             bool show=false)
{
    cv::Mat gray_img = cv::imread("assets/bathroom.jpg",
                                  cv::IMREAD_GRAYSCALE);

    cv::Mat noise_img(gray_img.size(), gray_img.type());

    cv::Mat angle_img(gray_img.size(), CV_64F);
    cv::Mat gradient_img(gray_img.size(), CV_64F);
    angles = new_image_double(gray_img.cols, gray_img.rows);
    modgrad = new_image_double(gray_img.cols, gray_img.rows);

    grad_x = cv::Mat(gray_img.size(), CV_64F);
    grad_y = cv::Mat(gray_img.size(), CV_64F);
    bad_pixels = cv::Mat(gray_img.size(), CV_8U);

    long long total_time = 0;
    for (int i = 0; i < num_tests; ++i)
    {
        // Do a pass with random noise to avoid any caching effects
        cv::randn(noise_img, 125, 50);
        angle_gradient(noise_img, angle_img, gradient_img);

        auto start = high_resolution_clock::now();
        angle_gradient(gray_img, angle_img, gradient_img);
        auto end = high_resolution_clock::now();
        total_time += duration_cast<microseconds>(end - start).count();
    }
    auto mean_time = total_time / static_cast<long>(num_tests);
    printf("%s: %lld microseconds per call\n", func_name.c_str(), mean_time);

    if(show)
    {
        cv::namedWindow(func_name, cv::WINDOW_NORMAL);
        cv::imshow(func_name, draw_map(gradient_img, false));
    }

    free_image_double(angles);
    free_image_double(modgrad);

    return gradient_img;
}



void opencv_angle_gradient(const cv::Mat& gray,
                              cv::Mat& ang_img,
                              cv::Mat& grad_img) {
    auto img_height = gray.rows;
    auto img_width = gray.cols;
    constexpr double NOTDEF = -1024.0;
    constexpr double DEG_TO_RADS = CV_PI / 180;
    auto threshold = 5.2262518595055063;

    // Undefined the down and right boundaries
    ang_img.row(img_height - 1).setTo(NOTDEF);
    ang_img.col(img_width - 1).setTo(NOTDEF);

    // Computing gradient for remaining pixels
    for(int y = 0; y < img_height - 1; ++y)
    {
        auto* scaled_image_row = gray.ptr<uchar>(y);
        auto* next_scaled_image_row = gray.ptr<uchar>(y+1);
        auto* angles_row = ang_img.ptr<double>(y);
        auto* modgrad_row = grad_img.ptr<double>(y);
        for(int x = 0; x < img_width-1; ++x)
        {
            int DA = next_scaled_image_row[x + 1] - scaled_image_row[x];
            int BC = scaled_image_row[x + 1] - next_scaled_image_row[x];
            int gx = DA + BC;    // gradient x component
            int gy = DA - BC;    // gradient y component
            double norm = std::sqrt((gx * gx + gy * gy) / 4.0); // gradient norm

            modgrad_row[x] = norm;    // store gradient

            if (norm <= threshold)  // norm too small, gradient no defined
            {
                angles_row[x] = NOTDEF;
            }
            else
            {
                angles_row[x] = cv::fastAtan2(float(gx), float(-gy)) * DEG_TO_RADS;  // gradient angle computation
            }

        }
    }
}

void pytlsd_angle_gradient(const cv::Mat& gray,
                           cv::Mat& ang_img,
                           cv::Mat& grad_img) {

    cv::Mat img_flt;
    gray.convertTo(img_flt, CV_64F);
    auto *imagePtr = reinterpret_cast<double *>(img_flt.data);
    image_double image = new_image_double_ptr(gray.cols, gray.rows, imagePtr);

    auto threshold = 5.2262518595055063;
    grad_angle_orientation(image, threshold, angles, modgrad);
    memcpy(ang_img.data, angles->data, ang_img.cols * ang_img.rows * sizeof(double));
    memcpy(grad_img.data, modgrad->data, grad_img.cols * grad_img.rows * sizeof(double));


}

void custom_angle_gradient(const cv::Mat& gray,
                              cv::Mat& ang_img,
                              cv::Mat& grad_img) {
    auto *imagePtr = reinterpret_cast<unsigned char *>(gray.data);
    auto *grad_xPtr = reinterpret_cast<double *>(grad_x.data);
    auto *grad_yPtr = reinterpret_cast<double *>(grad_y.data);
    auto *magnitudesPtr = reinterpret_cast<double *>(grad_img.data);
    auto *bad_pixelsPtr = reinterpret_cast<unsigned char *>(bad_pixels.data);

    gradientCalculator->calculateGradients(imagePtr,
                                           gray.cols, gray.rows,
                                           grad_xPtr, grad_yPtr,
                                           magnitudesPtr, bad_pixelsPtr);
}

int main() {
    auto num_test = 10;
    auto opencv_gradient_img = test(opencv_angle_gradient,
                                    "opencv_angle_gradient",
                                    num_test,
                                    true);

    double threshold = 5.2262518595055063;
    gradientCalculator = std::make_unique<GradientCalculator>(threshold);
    auto custom_gradient_img = test(custom_angle_gradient,
                                     "custom_angle_gradientr",
                                     num_test);

    auto pytlsd_gradient_img = test(pytlsd_angle_gradient,
                                    "pytlsd_angle_gradient",
                                    num_test);

    auto diff1 = draw_image_diff(pytlsd_gradient_img,
                                 opencv_gradient_img,
                                 "Pytlsd Vs Opencv",
                                 true);

    auto diff2 = draw_image_diff(custom_gradient_img,
                                 opencv_gradient_img,
                                 "SepConv Vs Opencv",
                                 true);

    cv::Mat grad_color = draw_map(opencv_gradient_img);
    cv::Mat combined_diff;
    cv::hconcat(grad_color, diff2, combined_diff);
    cv::hconcat(combined_diff, diff2, combined_diff);
    cv::imwrite("../../doc/img/gradient_diff.png", combined_diff);

    cv::waitKey(0);
    return 0;
}