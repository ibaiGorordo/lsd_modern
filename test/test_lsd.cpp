#include <iostream>
#include "chrono"

#include <opencv2/opencv.hpp>

#include "lsd.h"
#include "LineSegmentDetector.h"
#include "test_utils.h"

using namespace std::chrono;
using Line = std::pair<cv::Point2d, cv::Point2d>;

cv::Ptr<cv::LineSegmentDetector> lsd_cv;
std::unique_ptr<LineSegmentDetector> lsd_modern;

cv::Mat test(cv::Mat& gray_img,
             const std::function<std::vector<Line>(cv::Mat&)>& lsd,
             const std::string& func_name,
             int num_tests)
{
    auto start = high_resolution_clock::now();
    std::vector<Line> lines;
    for (int i = 0; i < num_tests; ++i)
    {
        lines = lsd(gray_img);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    auto mean_time = duration.count() / static_cast<long>(num_tests);
    printf("%s: %lld ms per call\n", func_name.c_str(), mean_time);

    return draw_lines(gray_img, lines, func_name);
}

std::vector<Line> pytlsd(cv::Mat& gray_img)
{
    cv::Mat img_flt;
    gray_img.convertTo(img_flt, CV_64FC1);

    auto *imagePtr = reinterpret_cast<double *>(img_flt.data);

    // LSD call. Returns [x1,y1,x2,y2,width,p,-log10(NFA)] for each segment
    int N;
    double *out = lsd(&N, imagePtr, img_flt.cols, img_flt.rows);

    std::vector<Line> lines;
    for (int i = 0; i < N; ++i)
    {
        double x1 = out[5 * i];
        double y1 = out[5 * i + 1];
        double x2 = out[5 * i + 2];
        double y2 = out[5 * i + 3];
        lines.emplace_back(cv::Point2d(x1, y1), cv::Point2d(x2, y2));
    }

    free(out);
    return lines;
}

std::vector<Line> opencv_lsd(cv::Mat& gray_img)
{
    std::vector<cv::Vec4f> lines_cv;
    lsd_cv->detect(gray_img, lines_cv);

    std::vector<Line> lines;
    for (const auto& line : lines_cv)
    {
        lines.emplace_back(cv::Point2d(line[0], line[1]),
                            cv::Point2d(line[2], line[3]));
    }

    return lines;
}

std::vector<Line> modern_lsd(cv::Mat& gray_img)
{
    cv::Mat img_flt;
    gray_img.convertTo(img_flt, CV_64FC1);

    auto *imagePtr = reinterpret_cast<double *>(img_flt.data);
    auto segments = lsd_modern->detect(imagePtr, gray_img.cols, gray_img.rows);

    std::vector<Line> lines;
    for (const auto& segment : segments)
    {
        lines.emplace_back(cv::Point2d(segment.x1, segment.y1),
                           cv::Point2d(segment.x2, segment.y2));
    }
    return lines;
}


int main(){
    std::vector<std::string> images{"building",
                                    "eiffel",
                                    "living_room",
                                    "zurich",
                                    "bathroom"};

    for(const auto& img_name : images){
        cv::Mat gray = cv::imread("assets/" + img_name + ".jpg",
                                  cv::IMREAD_GRAYSCALE);

        auto num_tests = 1;
        lsd_cv = cv::createLineSegmentDetector();
        lsd_modern = std::make_unique<LineSegmentDetector>();
        auto line_img1 = test(gray, pytlsd, "pytlsd", num_tests);
        auto line_img2 = test(gray, opencv_lsd, "opencv_lsd", num_tests);
        auto line_img3 = test(gray, modern_lsd, "modern_lsd", num_tests);

        // Stack images horizontally
        cv::Mat line_img;
        cv::hconcat(line_img1, line_img2, line_img);
        cv::imwrite("../../doc/img/lines.jpg", line_img);
        cv::namedWindow("lines", cv::WINDOW_NORMAL);
        cv::imshow("lines", line_img);
        cv::waitKey(0);
    }


}