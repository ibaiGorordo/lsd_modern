//
// Created by Ibai Gorordo on 2023/02/17.
//
#include "cmath"

#include "RegionRect.h"
#include "utils.h"

RegionRect::RegionRect(const std::vector<RegionPoint> &region_points,
                       double reg_angle, double prec, double p) {


    const auto centroid = getRegCentroid(region_points);

    this->theta = getRegAngle(region_points, centroid, reg_angle, prec);
    this->dx = cos(this->theta);
    this->dy = sin(this->theta);

    const auto regionSize = getRegionSize(region_points, centroid, dx, dy);
    this->x1 = centroid.x + regionSize.lmin * dx;
    this->y1 = centroid.y + regionSize.lmin * dy;
    this->x2 = centroid.x + regionSize.lmax * dx;
    this->y2 = centroid.y + regionSize.lmax * dy;
    this->width = regionSize.wmax - regionSize.wmin;
    this->prec = prec;
    this->p = p;

    if(this->width < 1.0) this->width = 1.0;
}

Centroid getRegCentroid(const std::vector<RegionPoint> &region_points) {

    Centroid centroid;

    double sum_norm = 0;
    for (const auto &region_point : region_points) {
        if(!region_point.valid) continue;

        const auto norm = region_point.norm;
        sum_norm += norm;
        centroid.x += region_point.x * norm;
        centroid.y += region_point.y * norm;
    }

    if (sum_norm == 0) {
        return centroid;
    }

    centroid.x /= sum_norm;
    centroid.y /= sum_norm;

    return centroid;
}

double getRegAngle(const std::vector<RegionPoint> &region_points,
                            const Centroid &centroid, double reg_angle,
                            double prec) {

    const auto inertiaMatrix = getInertiaMatrix(region_points, centroid);
    const auto eigen_value = calculateEigenValue(inertiaMatrix);

    double angle = std::fabs(inertiaMatrix.Ixx) > std::fabs(inertiaMatrix.Iyy) ?
                         fast_atan2d(eigen_value - inertiaMatrix.Ixx, inertiaMatrix.Ixy) :
                         fast_atan2d(inertiaMatrix.Ixy, eigen_value - inertiaMatrix.Iyy);

    // Align angle with reg_angle
    if(angle_diff(angle, reg_angle) > prec) angle-=pi;

    return angle;
}

RegionSize getRegionSize(const std::vector<RegionPoint> &regionPoints,
                         const Centroid &centroid, double dx, double dy) {

    RegionSize regionSize;

    for (const auto &region_point : regionPoints) {
        if(!region_point.valid) continue;

        const auto regdx = region_point.x - centroid.x;
        const auto regdy = region_point.y - centroid.y;
        const auto l = regdx * dx + regdy * dy;
        const auto w = -regdx * dy + regdy * dx;

        if (l < regionSize.lmin) regionSize.lmin = l;
        else if (l > regionSize.lmax) regionSize.lmax = l;
        if (w < regionSize.wmin) regionSize.wmin = w;
        else if (w > regionSize.wmax) regionSize.wmax = w;
    }

    return regionSize;
}

InertiaMatrix getInertiaMatrix(const std::vector<RegionPoint> &regionPoints,
                             const Centroid &centroid) {

    InertiaMatrix inertia_matrix;

    for (const auto &region_point : regionPoints) {
        if(!region_point.valid) continue;

        const auto norm = region_point.norm;
        const auto regx = region_point.x;
        const auto regy = region_point.y;
        const auto dx = regx - centroid.x;
        const auto dy = regy - centroid.y;
        inertia_matrix.Ixx += norm * dy * dy;
        inertia_matrix.Iyy += norm * dx * dx;
        inertia_matrix.Ixy -= norm * dx * dy;
    }

    return inertia_matrix;
}
double calculateEigenValue(const InertiaMatrix &inertiaMatrix) {

    return 0.5 * (inertiaMatrix.Ixx + inertiaMatrix.Iyy +
                  std::sqrt((inertiaMatrix.Ixx - inertiaMatrix.Iyy) *
                       (inertiaMatrix.Ixx - inertiaMatrix.Iyy) +
                       4 * inertiaMatrix.Ixy * inertiaMatrix.Ixy));
}

double getRegionDensity(const RegionRect &regionRect, size_t num_points) {
    const auto length = point_dist(regionRect.x1, regionRect.y1, regionRect.x2, regionRect.y2);
    if (length < 1e-6) return 0;

    return static_cast<double>(num_points) / (length * regionRect.width);
}
