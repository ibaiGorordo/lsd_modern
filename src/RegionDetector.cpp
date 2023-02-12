//
// Created by ibaig on 1/3/2023.
//
#include <vector>
#include <algorithm>
#include <cmath>
#include <numbers>

#include "RegionDetector.h"
#include "utils.h"

RegionDetector::RegionDetector(double thres) {
    ang_th = thres * std::numbers::pi / 180.0;
}

void RegionDetector::detect(const double *angles,
                            const double *magnitudes,
                            unsigned char *bad_pixels,
                            int width, int height) {
    check_new_img_size(width, height);

    angles_ptr = angles;
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
    for(int y = 0; y < img_height; y++)
    {
        auto row_start = y * img_width;
        for(int x = 0; x < img_width; x++)
        {
            int index = row_start + x;
            if(!used_pixels_ptr[index])
            {
                auto quant_norm = static_cast<uint16_t>(magnitudes_ptr[index] * quant_coeff +0.5);
                if (quant_norm == 0)
                    continue;
                sorted_pixels.emplace_back(x, y, quant_norm);
            }
        }
    }

    // Sort in descending order
    std::sort(sorted_pixels.begin(), sorted_pixels.end(), std::greater<>());
}

void RegionDetector::search_regions() {
    region_count = 0;
//    printf("First pixel: %d, %d\n", sorted_pixels[0].x, sorted_pixels[0].y);
    for(auto& point : sorted_pixels)
    {
        if(used_pixels_ptr[point.y * img_width + point.x])
            continue;

        reset_region();
        region_grow(point.x, point.y);

        if(region_points.size() < min_reg_size) continue;
//        printf("region:%d, point: %d, %d, Found region lsd of size %zu\n",
//               region_count, point.x, point.y, region_points.size());
        region_count++;
    }

    printf("Found %d regions\n", region_count);
}

void RegionDetector::region_grow(int x, int y) {


    register_point(x, y);
    for(size_t i = 0; i < region_points.size(); i++) {
        auto &point = region_points[i];

        int xx_min = min_limit(point.x);
        int xx_max = max_limit(point.x, img_width);
        int yy_min = min_limit(point.y);
        int yy_max = max_limit(point.y, img_height);

        // Check the 8 neighbors
        for (int y_neigh = yy_min; y_neigh <= yy_max; y_neigh++) {
            const auto row_start = y_neigh * img_width;
            for (int x_neigh = xx_min; x_neigh <= xx_max; x_neigh++) {
                int index = row_start + x_neigh;

                if (used_pixels_ptr[index])
                  continue;

                const auto angle = angles_ptr[index];

                if (!is_aligned(angle, reg_angle, ang_th))
                    continue;
                register_point(x_neigh, y_neigh);
            }
        }
    }
}

// TODO: log_nt is also used in nfa, so maybe get the log_nt as input instead of calculating it here and in nfa
void RegionDetector::check_new_img_size(int width, int height) {
    if (img_width == width && img_height == height) return;

    img_width = width;
    img_height = height;
    auto log_nt = 5 * (std::log10(double(img_width)) + std::log10(double(img_height))) / 2;
    min_reg_size = static_cast<int>(-log_nt / std::log10(ang_th/std::numbers::pi));
}

void RegionDetector::register_point(int x, int y) {
    auto index = y * img_width + x;
    auto angle = angles_ptr[index];
    reg_dx += std::cos(angle);
    reg_dy += std::sin(angle);
    reg_angle = fast_atan2f((float) reg_dy, (float) reg_dx);
    auto norm = magnitudes_ptr[index];
    region_points.emplace_back(x, y, angle, norm);
    used_pixels_ptr[index] = 1;
}

bool RegionDetector::is_aligned(double angle, double reg_angle, double threshold) {

    auto theta = reg_angle - angle;
    theta = wrap_angle(theta);

    return  theta < threshold;
}


void RegionDetector::reset_region() {
    region_points.clear();
    reg_dx = 0;
    reg_dy = 0;
}




