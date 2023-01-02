//
// Created by ibaig on 12/30/2022.
//
#include "chrono"
#include <opencv2/opencv.hpp>

#include "lsd.h"
#include "GaussianDownsampler.h"

using namespace std::chrono;

std::unique_ptr<GaussianDownsampler> gaussian_downsampler;

cv::Mat test(const std::function<cv::Mat(cv::Mat&)>& gaussian_resize,
             const std::string& func_name,
             int num_tests,
             bool show=false)
{
    cv::Mat out_img;
    cv::Mat gray_img = cv::imread("assets/bathroom.jpg",
                                  cv::IMREAD_GRAYSCALE);

    cv::Mat noise_img(gray_img.size(), gray_img.type());

    long long total_time = 0;
    for (int i = 0; i < num_tests; ++i)
    {
        // Do a pass with random noise to avoid any caching effects
        cv::randn(noise_img, 125, 50);
        out_img = gaussian_resize(noise_img);

        auto start = high_resolution_clock::now();
        out_img = gaussian_resize(gray_img);
        auto end = high_resolution_clock::now();
        total_time += duration_cast<microseconds>(end - start).count();
    }
    auto mean_time = total_time / static_cast<long>(num_tests);
    printf("%s: %lld microseconds per call\n", func_name.c_str(), mean_time);

    if (show)
    {
        cv::namedWindow(func_name, cv::WINDOW_NORMAL);
        cv::imshow(func_name, out_img);
    }

    return out_img;
}

cv::Mat draw_image_diff(cv::Mat& img1,
                        cv::Mat& img2,
                        const std::string& title="diff",
                        bool show=false)
{
    cv::Mat diff;
    cv::absdiff(img1, img2, diff);

    // Normalize the difference with the min max to apply a colormap
    double min, max;
    cv::minMaxLoc(diff, &min, &max);
    cv::Mat adjMap;
    diff.convertTo(adjMap, CV_8UC1, 255 / (max-min), -min);

    // Apply the colormap
    cv::Mat diff_color;
    cv::applyColorMap(adjMap, diff_color, cv::COLORMAP_JET);

    if(show)
    {
        cv::namedWindow(title, cv::WINDOW_NORMAL);
        cv::imshow(title, diff_color);
    }
    return diff_color;
}

cv::Mat opencv_gaussian_resize(const cv::Mat& gray) {
    // Test Opencv Gaussian Blur
    cv::Mat opencv_gaussian_img, opencv_gaussian_img_resized;
    auto scale = 0.8;
    auto sigma = 0.6/0.8;
    const double sprec = 3;
    const auto h =  (int)(ceil(sigma * sqrt(2 * sprec * log(10.0))));
    cv::Size ksize(1 + 2 * h, 1 + 2 * h); // kernel size
    cv::GaussianBlur(gray, opencv_gaussian_img, ksize, sigma);

    cv::Size new_size(int(ceil(gray.cols * scale)),
                      int(ceil(gray.rows * scale)));
    resize(opencv_gaussian_img, opencv_gaussian_img_resized, new_size, 0, 0, cv::INTER_LINEAR_EXACT);

    return opencv_gaussian_img_resized;
}

cv::Mat pytlsd_gaussian_resize(const cv::Mat& gray) {

    auto scale = 0.8;
    auto new_width = (int) ceil(gray.cols * scale);
    auto new_height = (int) ceil(gray.rows * scale);

    cv::Mat img_flt;
    gray.convertTo(img_flt, CV_64F);
    auto *imagePtr = reinterpret_cast<double *>(img_flt.data);
    image_double image = new_image_double_ptr(gray.cols, gray.rows, imagePtr);

    cv::Mat out_img;
    out_img = cv::Mat::zeros(new_height, new_width, CV_64F);
    auto out_image_double = gaussian_sampler(image, scale, 0.6);
    out_img.data = reinterpret_cast<uchar *>(out_image_double->data);

    cv::Mat out_img_8u;
    out_img.convertTo(out_img_8u, CV_8U);
    free((void *) out_image_double->data);
    free((void *) out_image_double);
    return out_img_8u;
}

cv::Mat custom_gaussian_resize(const cv::Mat& gray) {
    auto *imagePtr = reinterpret_cast<unsigned char *>(gray.data);

    auto new_width = static_cast<int>(std::ceil(gray.cols * gaussian_downsampler->scale));
    auto new_height = static_cast<int>(std::ceil(gray.rows * gaussian_downsampler->scale));
    cv::Mat out_img(new_height, new_width, CV_8U);
    auto *outPtr = reinterpret_cast<unsigned char *>(out_img.data);

    gaussian_downsampler->blur_downsample(imagePtr,
                               outPtr,
                               gray.cols, gray.rows);
    return out_img;
}


int main() {
    auto num_test = 10;
    auto opencv_gaussian_resize_img = test(opencv_gaussian_resize,
                                           "opencv_gaussian_resize",
                                           num_test);

    gaussian_downsampler = std::make_unique<GaussianDownsampler>(0.8f, 0.6f);
    auto sepconv_gaussian_resize_img = test(custom_gaussian_resize,
                                            "custom_gaussian_resize",
                                            num_test,
                                            true);

    auto pytlsd_gaussian_resize_img = test(pytlsd_gaussian_resize,
                                           "pytlsd_gaussian_resize",
                                           num_test);

    auto diff1 = draw_image_diff(pytlsd_gaussian_resize_img,
                                 opencv_gaussian_resize_img,
                                 "Pytlsd Vs Opencv",
                                 true);

    auto diff2 = draw_image_diff(sepconv_gaussian_resize_img,
                                 opencv_gaussian_resize_img,
                                 "SepConv Vs Opencv",
                                 true);

    cv::Mat combined_diff;
    cv::hconcat(diff1, diff2, combined_diff);
    cv::imwrite("../../doc/img/gaussian_blur_resize_diff.png", combined_diff);

    cv::waitKey(0);
    return 0;
}
