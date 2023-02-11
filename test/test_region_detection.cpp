//
// Created by ibaig on 1/3/2023.
//
#include <memory>
#include <chrono>

#include "lsd.h"

#include <opencv2/opencv.hpp>

#include "GradientCalculator.h"
#include "RegionDetector.h"

using namespace std::chrono;

struct normPoint
{
    cv::Point2i p;
    int norm{};
};

static inline bool compare_norm( const normPoint& n1, const normPoint& n2 )
{
    return (n1.norm > n2.norm);
}


std::unique_ptr<GradientCalculator> gradientCalculator;
std::unique_ptr<RegionDetector> regionDetector;
std::vector<normPoint> ordered_points;

cv::Mat gradx_img, grady_img, magnitude_img, ang_img, bad_pixels_img, resized_img;


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
    cv::Mat res_img;
    cv::GaussianBlur(input_img, res_img,
                     cv::Size(ksize,ksize), sigma);

    cv::Size new_size(int(ceil(input_img.cols * scale)),
                      int(ceil(input_img.rows * scale)));
    resize(res_img, res_img, new_size, 0, 0, cv::INTER_LINEAR_EXACT);

    return res_img;
}


void calculate_gradient()
{
    auto threshold = 5.2262518595055063;
    if(gradientCalculator == nullptr)
    {
        gradientCalculator = std::make_unique<GradientCalculator>(threshold);
    }

    gradx_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    grady_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    magnitude_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    ang_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    bad_pixels_img = cv::Mat(resized_img.rows, resized_img.cols, CV_8U, 0.0);

    auto *imagePtr = reinterpret_cast<unsigned char *>(resized_img.data);
    auto *grad_xPtr = reinterpret_cast<double *>(gradx_img.data);
    auto *grad_yPtr = reinterpret_cast<double *>(grady_img.data);
    auto *magnitudesPtr = reinterpret_cast<double *>(magnitude_img.data);
    auto *anglesPtr = reinterpret_cast<double *>(ang_img.data);
    auto *bad_pixelsPtr = reinterpret_cast<unsigned char *>(bad_pixels_img.data);

    constexpr double DEG_TO_RADS = CV_PI / 180;
    gradientCalculator->calculateGradients(imagePtr,
                                           resized_img.cols, resized_img.rows,
                                           grad_xPtr, grad_yPtr,
                                           magnitudesPtr, bad_pixelsPtr);

    // Calculate the ang img using the grad_x and grad_y
    auto max_grad = 0.0;
    for(int y = 0; y < resized_img.rows; ++y)
    {
        auto* angles_row = ang_img.ptr<double>(y);
        for(int x = 0; x < resized_img.cols; ++x)
        {
            auto gx = grad_xPtr[y * resized_img.cols + x];
            auto gy = grad_yPtr[y * resized_img.cols + x];
            auto mag = magnitudesPtr[y * resized_img.cols + x];
            if(mag > max_grad)
            {
                max_grad = mag;
            }
            if(bad_pixelsPtr[y * resized_img.cols + x] == 1)
            {
                angles_row[x] = -1024.0;
                continue;
            }
            angles_row[x] = cv::fastAtan2(float(gx), float(-gy)) * DEG_TO_RADS;
        }
    }

    // Compute histogram of gradient values
    ordered_points.clear();
    ordered_points.reserve(resized_img.rows * resized_img.cols);
    int n_bins = 1024;
    double bin_coef = (max_grad > 0) ? double(n_bins - 1) / max_grad : 0; // If all image is smooth, max_grad <= 0
    for(int y = 0; y < resized_img.rows - 1; ++y)
    {
        const double* modgrad_row = magnitude_img.ptr<double>(y);
        for(int x = 0; x < resized_img.cols - 1; ++x)
        {
            normPoint _point;
            int i = int(modgrad_row[x] * bin_coef);
            _point.p = cv::Point(x, y);
            _point.norm = i;
            ordered_points.push_back(_point);
        }
    }

    // Sort
    std::sort(ordered_points.begin(), ordered_points.end(), compare_norm);
}

void test(const std::function<void()>& region_detect,
          const std::string& func_name,
          int num_tests)
{
    cv::Mat gray_img = cv::imread("assets/bathroom.jpg",
                                  cv::IMREAD_GRAYSCALE);

    resized_img = gaussian_resize(gray_img);

    long long total_time = 0;
    for (int i = 0; i < num_tests; ++i)
    {
        calculate_gradient();

        auto start = high_resolution_clock::now();
        region_detect();
        auto end = high_resolution_clock::now();
        total_time += duration_cast<microseconds>(end - start).count();
    }
    auto mean_time = total_time / static_cast<long>(num_tests);
    printf("%s: %lld microseconds per call\n", func_name.c_str(), mean_time);
}


void test_custom_region_detector()
{
    regionDetector->detect(gradx_img.ptr<double>(),
                           grady_img.ptr<double>(),
                           magnitude_img.ptr<double>(),
                           bad_pixels_img.ptr<unsigned char>(),
                           resized_img.cols, resized_img.rows);

}

void test_pytlsd_region_detector()
{
    struct point *reg;
    image_char used;
    image_double angles, modgrad;

    auto *magnitudesPtr = reinterpret_cast<double *>(magnitude_img.data);
    auto *anglesPtr = reinterpret_cast<double *>(ang_img.data);
    angles = new_image_double_ptr(resized_img.cols, resized_img.rows, anglesPtr);
    used = new_image_char_ini(ang_img.cols, ang_img.rows, 0);

    reg = (struct point *) calloc( (size_t) (ang_img.cols * ang_img.rows), sizeof(struct point) );
    int reg_size;
    double reg_angle, prec;

    /* angle tolerance */
    auto ang_th = 22.5;
    auto p = ang_th / 180.0;
    prec = M_PI * ang_th / 180.0;

    auto logNT = 5.0 * ( log10( (double) ang_img.cols ) + log10( (double) ang_img.rows ) ) / 2.0;
    auto min_reg_size = (int) (-logNT / log10(p));

    printf("First pixel: %d, %d\n", ordered_points[0].p.x, ordered_points[0].p.y);

    // Search for line segments
    for(size_t i = 0, points_size = ordered_points.size(); i < points_size; ++i) {
        const cv::Point2i &point = ordered_points[i].p;
        if (bad_pixels_img.at<unsigned char>(point) == 1) continue;
        region_grow(ordered_points[i].p.x, ordered_points[i].p.y, angles, reg, &reg_size, &reg_angle, used, prec);
        if(reg_size>= min_reg_size)
        {
            printf("Found region of size %d\n", reg_size);
        }
        break;

    }

    free_image_char(used);
}

int main() {

    auto num_test = 1;
    auto ang_thres = 22.5;
    regionDetector = std::make_unique<RegionDetector>(ang_thres);
    test(test_pytlsd_region_detector, "pytlsd_region_detector", num_test);
    test(test_custom_region_detector, "custom_region_detector", num_test);

    auto magnitude_color = draw_map(ang_img, false);
    cv::namedWindow("magnitude", cv::WINDOW_NORMAL);
    cv::imshow("magnitude", magnitude_color);
    cv::waitKey(0);

    return 0;
}
