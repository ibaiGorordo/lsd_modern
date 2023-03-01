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

    magnitude_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, 0.0);
    ang_img = cv::Mat(resized_img.rows, resized_img.cols, CV_64F, -1024.0);
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
    regionDetector->detect(ang_img.ptr<double>(),
                           magnitude_img.ptr<double>(),
                           bad_pixels_img.ptr<unsigned char>(),
                           resized_img.cols, resized_img.rows);

}

void test_pytlsd_region_detector()
{
    struct point *reg;
    image_char used;
    image_double angles, modgrad;

    auto max_grad = 0.0;
    for(int y = 0; y < resized_img.rows; ++y)
    {
        const double* modgrad_row = magnitude_img.ptr<double>(y);
        for(int x = 0; x < resized_img.cols; ++x)
        {
            if(modgrad_row[x] > max_grad)
                max_grad = modgrad_row[x];
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

    auto *magnitudesPtr = reinterpret_cast<double *>(magnitude_img.data);
    auto *anglesPtr = reinterpret_cast<double *>(ang_img.data);
    angles = new_image_double_ptr(resized_img.cols, resized_img.rows, anglesPtr);
    modgrad = new_image_double_ptr(resized_img.cols, resized_img.rows, magnitudesPtr);
    used = new_image_char_ini(ang_img.cols, ang_img.rows, 0);

    reg = (struct point *) calloc( (size_t) (ang_img.cols * ang_img.rows), sizeof(struct point) );
    int reg_size;
    double reg_angle, prec;
    struct rect rec;

    /* angle tolerance */
    auto ang_th = 22.5;
    auto density_th = 0.7;
    auto p = ang_th / 180.0;
    prec = M_PI * ang_th / 180.0;

    auto logNT = 5.0 * ( log10( (double) ang_img.cols ) + log10( (double) ang_img.rows ) ) / 2.0;
    auto min_reg_size = (int) (-logNT / log10(p));

//    printf("First pixel: %d, %d\n", ordered_points[0].p.x, ordered_points[0].p.y);
    auto region_count = 0;
    // Search for line segments
    for(size_t i = 0, points_size = ordered_points.size(); i < points_size; ++i) {
        const cv::Point2i &point = ordered_points[i].p;
        if (bad_pixels_img.data[point.x + point.y * bad_pixels_img.cols] == 1)
            continue;

        if (used->data[point.x + point.y * used->xsize] == 1)
            continue;

        region_grow(point.x, point.y, angles, reg, &reg_size, &reg_angle, used, prec);
        /* reject small regions */
        if ( reg_size < min_reg_size ) continue;

        /* construct rectangular approximation for the region */
        region2rect(reg, reg_size, modgrad, reg_angle, prec, p, &rec);
        
        if ( !refine( reg, &reg_size, modgrad, reg_angle,
                    prec, p, &rec, used, angles, density_th ) ) continue;

        region_count++;
    }

    printf("Found %d regions\n", region_count);

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
