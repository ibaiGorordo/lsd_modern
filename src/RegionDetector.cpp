//
// Created by ibaig on 1/3/2023.
//
#include <vector>
#include <algorithm>
#include <cmath>
#include <numbers>

#include "PixelSorter.h"
#include "RegionDetector.h"
#include "RegionRect.h"
#include "utils.h"

RegionDetector::RegionDetector(double thres, double density_threshold) {
    this->angTh = thres * std::numbers::pi / 180.0;
    this->angThNorm = thres / 180.0;
    this->densityTh = density_threshold;
}

void RegionDetector::detect(const double *angles,
                            const double *magnitudes,
                            unsigned char *bad_pixels,
                            int width, int height) {
    checkNewImgSize(width, height);

    anglesPtr = angles;
    magnitudesPtr = magnitudes;
    usedPixelsPtr = bad_pixels;

    // Get the sorted pixel coordinates bsaed on the magnitude that are not bad pixels
    PixelSorter::sort(sorted_pixels, magnitudesPtr, usedPixelsPtr, imgWidth, imgHeight);

    // Search regions
    searchRegions();

}

void RegionDetector::searchRegions() {
    regionCount = 0;

    for(auto& point : sorted_pixels)
    {
        if(usedPixelsPtr[point.y * imgWidth + point.x])
            continue;

        findRegion(point.x, point.y, angTh, minRegSize);
        if (!regionFound) continue ;

        refineRegion();
        if(regDensity < densityTh) continue;

        // Rect improve


        regionCount++;
    }

    printf("Found %d regions\n", regionCount);
}

void RegionDetector::refineRegion() {

    // Skip if already dense enough
    if(regDensity >= densityTh) return;

    // Try 1: Reduce angle tolerance
    // Recalculate the angle threshold by averaging the angle difference of the region points
    // near the seed point
    auto newAngTh = calculateRegionStdAngle()*2;
    auto seedPoint = region_points[0];
    findRegion(seedPoint.x, seedPoint.y, newAngTh, 2);

    if(regDensity >= densityTh || !regionFound) return;

    // Try 2: Reduce region radius

    // Compute the region radius
    auto xCenter = static_cast<double>(region_points[0].x);
    auto yCenter = static_cast<double>(region_points[0].y);
    auto radSq1 = point_dist_squared(xCenter, yCenter, regionRect.x1, regionRect.y1);
    auto radSq2 = point_dist_squared(xCenter, yCenter, regionRect.x2, regionRect.y2);
    auto radSq = radSq1 > radSq2 ? radSq1 : radSq2;

    while(regDensity < densityTh)
    {
        // Reduce the region radius
        radSq *= refineCoeffSq;

        // Set points outside the radius to as not valid
        for(auto& point : region_points)
        {
            if(!point.valid) continue;

            if (point_dist_squared(xCenter, yCenter, point.x, point.y) <= radSq)
                continue;

            point.valid = false;
            regionSize--;
            resetPoint(point);
        }

        // Check min region size 2
        if(region_points.size() < 2) return;

        calculateRect();
    }
}

void RegionDetector::calculateRect() {
    regionRect = RegionRect(region_points, regAngle, angTh, angThNorm);
    regDensity = getRegionDensity(regionRect, regionSize);
}

void RegionDetector::findRegion(int x, int y, double angleThreshold, int min_size) {
    resetRegion();
    regionGrow(x, y, angleThreshold);
    if(region_points.size() < min_size) return;
    regionFound = true;
    calculateRect();
}


void RegionDetector::regionGrow(int x, int y, double angle_threrehold) {


    registerPoint(x, y);
    for(size_t i = 0; i < region_points.size(); i++) {
        auto &point = region_points[i];

        int xx_min = minLimit(point.x);
        int xx_max = maxLimit(point.x, imgWidth);
        int yy_min = minLimit(point.y);
        int yy_max = maxLimit(point.y, imgHeight);

        // Check the 8 neighbors
        for (int y_neigh = yy_min; y_neigh <= yy_max; y_neigh++) {
            const auto row_start = y_neigh * imgWidth;
            for (int x_neigh = xx_min; x_neigh <= xx_max; x_neigh++) {
                int index = row_start + x_neigh;

                if (usedPixelsPtr[index])
                  continue;

                const auto angle = anglesPtr[index];

                if (!isAligned(angle, regAngle, angle_threrehold))
                    continue;
                registerPoint(x_neigh, y_neigh);
            }
        }
    }
}

double RegionDetector::calculateRegionStdAngle() {
    RegionPoint seedPoint = region_points[0];
    const auto seed_angle = seedPoint.angle;
    const auto seed_x = 463;//seedPoint.x;
    const auto seed_y = 401;//seedPoint.y;

    double sum_angle = 0;
    double squared_sum_angle = 0;
    int count = 0;

    for(auto& point : region_points)
    {
        resetPoint(point);
        if(point_dist( seed_x, seed_y, point.x, point.y) > regionRect.width)
            continue;

        auto angle = angle_diff_signed(point.angle, seed_angle);
        sum_angle += angle;
        squared_sum_angle += angle * angle;
        count++;
    }

    if(count == 0) return 0;

    auto mean_angle = sum_angle / count;
    return std::sqrt((squared_sum_angle - 2 * mean_angle * sum_angle) /
                                       count + mean_angle * mean_angle);
}

void RegionDetector::checkNewImgSize(int width, int height) {
    if (imgWidth == width && imgHeight == height) return;

    imgWidth = width;
    imgHeight = height;
    auto log_nt = 5 * (std::log10(double(imgWidth)) + std::log10(double(imgHeight))) / 2;
    minRegSize = static_cast<int>(-log_nt / std::log10(angTh /std::numbers::pi));
}

void RegionDetector::registerPoint(int x, int y) {
    auto index = y * imgWidth + x;
    auto angle = anglesPtr[index];
    regDx += std::cos(angle);
    regDy += std::sin(angle);
    regAngle = fast_atan2f((float) regDy, (float) regDx);
    auto norm = magnitudesPtr[index];
    region_points.emplace_back(x, y, angle, norm);
    regionSize++;
    usedPixelsPtr[index] = 1;
}

bool RegionDetector::isAligned(double angle, double reg_angle, double threshold) {

    auto theta = reg_angle - angle;
    theta = wrap_angle(theta, pi_and_half);

    return  theta < threshold;
}


void RegionDetector::resetRegion() {
    region_points.clear();
    regionFound = false;
    regionSize = 0;
    regDx = 0;
    regDy = 0;
    regAngle = 0;
    regDensity = 0;
    regionRect = RegionRect();
}
void RegionDetector::resetPoint(RegionPoint &point) {
    usedPixelsPtr[point.y * imgWidth + point.x] = 0;
}
