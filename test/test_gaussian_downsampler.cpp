//
// Created by ibaig on 12/30/2022.
//
#include "chrono"
#include <opencv2/opencv.hpp>

#include "lsd.h"
#include "GaussianDownsampler.h"

using namespace std::chrono;

std::unique_ptr<GaussianDownsampler> gaussian_downsampler;

cv::Mat test(const std::function<cv::Mat(cv::Mat&)>& gaussian_blur,
             const std::string& func_name,
             int num_tests,
             bool show=false)
{
    auto start = high_resolution_clock::now();
    cv::Mat out_img;
    for (int i = 0; i < num_tests; ++i)
    {
        cv::Mat gray_img = cv::imread("assets/test.jpg",
                                  cv::IMREAD_GRAYSCALE);
        out_img = gaussian_blur(gray_img);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    auto mean_time = duration.count() / static_cast<long>(num_tests);
    printf("%s: %lld ms per call\n", func_name.c_str(), mean_time);

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

cv::Mat opencv_gaussian_blur_resize(const cv::Mat& gray) {
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

cv::Mat pytlsd_gaussian_blur_resize(const cv::Mat& gray) {

    cv::Mat img_flt;
    gray.convertTo(img_flt, CV_64F);
    auto *imagePtr = reinterpret_cast<double *>(img_flt.data);

    auto scale = 0.8;

    cv::Mat out_img;
    auto new_width = (int) ceil(gray.cols * scale);
    auto new_height = (int) ceil(gray.rows * scale);
    out_img = cv::Mat::zeros(new_height, new_width, CV_64F);

    image_double image;
    image->data = imagePtr;
    image->xsize = gray.cols;
    image->ysize = gray.rows;
    auto out_image_double = gaussian_sampler(image, scale, 0.6);
    out_img.data = reinterpret_cast<uchar *>(out_image_double->data);

    cv::Mat out_img_8u;
    out_img.convertTo(out_img_8u, CV_8U);
    free((void *) out_image_double->data);
    free((void *) out_image_double);
    return out_img_8u;
}

cv::Mat sepconv_gaussian_blur_resize(const cv::Mat& gray) {
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
    auto opencv_gaussian_resize_img = test(opencv_gaussian_blur_resize,
                                           "opencv_gaussian_blur_resize",
                                           num_test);

    gaussian_downsampler = std::make_unique<GaussianDownsampler>(0.8f, 0.6f);
    auto sepconv_gaussian_resize_img = test(sepconv_gaussian_blur_resize,
                                            "sepconv_gaussian_blur_resize",
                                            num_test,
                                            true);

    auto pytlsd_gaussian_resize_img = test(pytlsd_gaussian_blur_resize,
                                           "pytlsd_gaussian_blur_resize",
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
