//
// Created by ibaig on 1/3/2023.
//
#include <vector>
#include <algorithm>
#include <cmath>
#include <numbers>

#include "RegionDetector.h"
#include "RegionRect.h"
#include "utils.h"

RegionDetector::RegionDetector(double thres, double density_threshold) {
    this->ang_th = thres * std::numbers::pi / 180.0;
    this->ang_th_norm = thres / 180.0;
    this->density_th = density_threshold;
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

void RegionDetector::search_regions() {
    region_count = 0;

    for(auto& point : sorted_pixels)
    {
        if(used_pixels_ptr[point.y * img_width + point.x])
            continue;

        find_region(point.x, point.y, ang_th, min_reg_size);

//        refine_region();

        if(reg_density < density_th)
            continue;

        // Rect improve

        region_count++;
    }

    printf("Found %d regions\n", region_count);
}

void RegionDetector::refine_region() {

    // Skip if already dense enough
    if(reg_density >= density_th) return;

    // Try 1: Reduce angle tolerance
    // Recalculate the angle threshold by averaging the angle difference of the region points
    // near the seed point
    auto new_ang_th = calculate_region_std_angle()*2;
    auto seed_point = region_points[0];
    find_region(seed_point.x, seed_point.y, new_ang_th, 2);

    if(reg_density >= density_th) return;

    // Try 2: Reduce region radius
}

void RegionDetector::find_region(int x, int y, double angle_threrehold, int min_size) {
    reset_region();
    region_grow(x, y, angle_threrehold);
    if(region_points.size() < min_size) return;
    region_rect = RegionRect(region_points, reg_angle, angle_threrehold, ang_th_norm);
    reg_density = getRegionDensity(region_rect, region_points.size());
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


void RegionDetector::region_grow(int x, int y, double angle_threrehold) {


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

                if (!is_aligned(angle, reg_angle, angle_threrehold))
                    continue;
                register_point(x_neigh, y_neigh);
            }
        }
    }
}

double RegionDetector::calculate_region_std_angle() {
    RegionPoint seedPoint = region_points[0];
    const auto seed_angle = seedPoint.angle;
    const auto seed_x = seedPoint.x;
    const auto seed_y = seedPoint.y;

    double sum_angle = 0;
    double squared_sum_angle = 0;
    int count = 0;

    for(auto& point : region_points)
    {
        reset_point(point);
        if(point_dist(point.x, point.y, seed_x, seed_y) > region_rect.width)
            continue;

        auto angle = angle_diff_signed(point.angle, seed_angle);
        sum_angle += angle;
        squared_sum_angle += angle * angle;
        count++;
    }

    if(count == 0) return 0;

    auto mean_angle = sum_angle / count;
    return std::sqrt((squared_sum_angle - 2 * mean_angle * sum_angle) /
                                       count - mean_angle * mean_angle);
}

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
    theta = wrap_angle(theta, pi_and_half);

    return  theta < threshold;
}


void RegionDetector::reset_region() {
    region_points.clear();
    reg_dx = 0;
    reg_dy = 0;
    reg_angle = 0;
    reg_density = 0;
    region_rect = RegionRect();
}
void RegionDetector::reset_point(RegionPoint &point) {
    used_pixels_ptr[point.y * img_width + point.x] = 0;
}
