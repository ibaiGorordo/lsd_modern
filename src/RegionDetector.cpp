//
// Created by ibaig on 1/3/2023.
//
#include <vector>
#include <algorithm>
#include <cmath>
#include <numbers>
#include "RegionDetector.h"

RegionDetector::RegionDetector(double ang_th) {
    this->tan_th = std::abs(tan(ang_th * std::numbers::pi / 180));
    p = ang_th / 180.0;
}

void RegionDetector::detect(const double *grad_x, const double *grad_y,
                            const double *magnitudes, unsigned char *bad_pixels,
                            int width, int height) {
    check_new_img_size(width, height);

    grad_x_ptr = grad_x;
    grad_y_ptr = grad_y;
    magnitudes_ptr = magnitudes;
    used_pixels_ptr = bad_pixels;

    // Get the sorted pixel coordinates bsaed on the magnitude that are not bad pixels
    get_sorted_pixels();

    // Search regions
    search_regions();

}

void RegionDetector::get_sorted_pixels()
{
    sorted_pixels.clear();
#pragma omp parallel for collapse(2)
    for(int y = 0; y < img_height; y++)
    {
        auto row_start = y * img_width;
        for(int x = 0; x < img_width; x++)
        {
            int index = row_start + x;
            if(!used_pixels_ptr[index])
            {
                auto quant_norm = static_cast<uint16_t>(magnitudes_ptr[index] * quant_coeff +0.5);
                sorted_pixels.emplace_back(x, y, quant_norm);
            }
        }
    }

    // Sort in descending order
    std::sort(sorted_pixels.begin(), sorted_pixels.end(), std::greater<>());
}

void RegionDetector::search_regions() {
    printf("First pixel: %d, %d\n", sorted_pixels[0].x, sorted_pixels[0].y);
    for(auto& point : sorted_pixels)
    {
        if(used_pixels_ptr[point.y * img_width + point.x])
            continue;

        reset_region();
        region_grow(point.x, point.y);

        if(region_points.size() < min_reg_size) continue;
        printf("Found region lsd of size %zu\n", region_points.size());
        break;
    }

}

void RegionDetector::region_grow(int x, int y) {

    register_point(x, y);

    int xx_min = min_limit(x);
    int xx_max = max_limit(x, img_width);
    int yy_min = min_limit(y);
    int yy_max = max_limit(y, img_height);

#pragma omp parallel for collapse(2)
    // Check the 8 neighbors
    for(int y_neigh = yy_min; y_neigh <= yy_max; y_neigh++)
    {
        const auto row_start = y_neigh * img_width;
        for(int x_neigh = xx_min; x_neigh <= xx_max; x_neigh++)
        {
            int index = row_start + x_neigh;

            if (used_pixels_ptr[index]) continue;

            auto dx_neigh = grad_x_ptr[index];
            auto dy_neigh = grad_y_ptr[index];

            if(!is_aligned(dx_neigh, dy_neigh, reg_dx, reg_dy, tan_th))
                continue;

            region_grow(x_neigh, y_neigh);
        }
    }
}

// TODO: log_nt is also used in nfa, so maybe get the log_nt as input instead of calculating it here and in nfa
void RegionDetector::check_new_img_size(int width, int height) {
    if (img_width == width && img_height == height) return;

    img_width = width;
    img_height = height;
    auto log_nt = 5 * (std::log10(double(img_width)) + std::log10(double(img_height))) / 2 + std::log10(11.0);
    min_reg_size = static_cast<int>(-log_nt / log10(p));
}

void RegionDetector::register_point(int x, int y) {
    auto index = y * img_width + x;
    auto dx = grad_x_ptr[index];
    auto dy = grad_y_ptr[index];
    reg_dx += dx;
    reg_dy += dy;
    auto norm = magnitudes_ptr[index];
    region_points.emplace_back(x, y, dx, dy, norm);
    used_pixels_ptr[index] = 1;
}

bool RegionDetector::is_aligned(double dx, double dy, double dx2, double dy2, double tan_th) {
    // Instead of using the angles, use dx/dy and dx2/dy2 to avoid the expensive atan2
    auto dot = dx * dx2 + dy * dy2;
    auto cross = dx * dy2 - dy * dx2;
    auto tan = cross / dot;
    return std::abs(tan) <= tan_th;
}

///** 3/2 pi */
//#define M_3_2_PI 4.71238898038
//
///** 2 pi */
//#define M_2__PI  6.28318530718
//
//bool RegionDetector::is_aligned(double dx, double dy, double dx2, double dy2, double tan_th) {
//    // Instead of using the angles, use dx/dy and dx2/dy2 to avoid the expensive atan2
//    auto dot = dx * dx2 + dy * dy2;
//    auto cross = dx * dy2 - dy * dx2;
//    auto tan = cross / dot;
//    auto angle1 = std::atan2(dy, dx);
//    auto angle2 = std::atan2(dy2, dx2);
//
//    auto theta = angle2 - angle1;
//    if ( theta < 0.0 ) theta = -theta;
//    if ( theta > M_3_2_PI )
//    {
//        theta -= M_2__PI;
//        if ( theta < 0.0 ) theta = -theta;
//    }
//
//    auto prec = M_PI * 22.5 / 180.0;
//    printf("theta: %f, atan:%f, tan1: %f, theta_smaller: %d, tan smaller: %d\n", theta, atan(tan), tan, theta < prec, std::abs(tan) < tan_th);
//    return  theta < prec;
//}


void RegionDetector::reset_region() {
    region_points.clear();
    reg_dx = 0;
    reg_dy = 0;
}




