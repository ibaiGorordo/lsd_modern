//
// Created by Ibai Gorordo on 2023/03/05.
//

#include "test_utils.h"


cv::Mat gaussian_resize(const cv::Mat& input_img)
{
    cv::Mat res_img;
    cv::GaussianBlur(input_img, res_img,
                     ksize, sigma);

    cv::Size new_size(int(floor(input_img.cols * scale)),
                      int(floor(input_img.rows * scale)));
    resize(res_img, res_img, new_size, 0, 0, cv::INTER_LINEAR_EXACT);

    return res_img;
}


void calculate_gradient(GradientCalculator* gradientCalculator,
                        const cv::Mat& resized_img,
                        cv::Mat& magnitude_img,
                        cv::Mat& ang_img,
                        cv::Mat& bad_pixels_img)
{
    magnitude_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    ang_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, bad_value);
    bad_pixels_img = cv::Mat(resized_img.rows, resized_img.cols, CV_8U, 0.0);

    auto *imagePtr = reinterpret_cast<unsigned char *>(resized_img.data);
    auto *magnitudesPtr = reinterpret_cast<double *>(magnitude_img.data);
    auto *anglesPtr = reinterpret_cast<double *>(ang_img.data);
    auto *bad_pixelsPtr = reinterpret_cast<unsigned char *>(bad_pixels_img.data);

    gradientCalculator->calculateGradients(imagePtr,
                                           resized_img.cols, resized_img.rows,
                                           anglesPtr, magnitudesPtr,
                                           bad_pixelsPtr);
}


cv::Mat draw_map(const cv::Mat& mat, bool use_min)
{
    cv::Mat adjMap;
    double min, max;
    cv::minMaxLoc(mat, &min, &max);

//    printf("min diff: %f, max diff: %f\n", min, max);

    if (!use_min) min = 0;

    if(max-min < 1e-6)
    {
        mat.convertTo(adjMap, CV_8UC1, 1, 0);
    }
    else
    {
        mat.convertTo(adjMap, CV_8UC1, 255 / (max-min), -min);
    }
    cv::applyColorMap(adjMap, adjMap, cv::COLORMAP_JET);
    return adjMap;
}

cv::Mat draw_image_diff(cv::Mat& img1,
                        cv::Mat& img2,
                        bool use_min,
                        const std::string& title,
                        bool show)
{
    cv::Mat diff;
    cv::absdiff(img1, img2, diff);

    // Apply the colormap
    cv::Mat diff_color = draw_map(diff, use_min);

    if(show)
    {
        cv::namedWindow(title, cv::WINDOW_NORMAL);
        cv::imshow(title, diff_color);
    }
    return diff_color;
}

cv::Mat draw_lines(cv::Mat& gray_img,
                   const std::vector<std::pair<cv::Point2d, cv::Point2d>>& lines,
                   const std::string& title,
                   bool show)
{
    cv::Mat color_img;
    cv::cvtColor(gray_img, color_img, cv::COLOR_GRAY2BGR);
    for (const auto& line : lines)
    {
        cv::line(color_img,
                 line.first,
                 line.second,
                 cv::Scalar(0, 0, 255),
                 2);
    }

    if(show)
    {
        cv::namedWindow(title, cv::WINDOW_NORMAL);
        cv::imshow(title, color_img);
    }
    return color_img;
}