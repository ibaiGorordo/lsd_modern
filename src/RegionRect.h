//
// Created by Ibai Gorordo on 2023/02/17.
//

#ifndef TEST_REGIONRECT_H
#define TEST_REGIONRECT_H

#include "RegionPoint.h"
#include <vector>

struct Centroid {
    double x=0;
    double y=0;
};

struct InertiaMatrix {
    double Ixx=0;
    double Ixy=0;
    double Iyy=0;
};

struct RegionSize {
    double lmin=0;
    double lmax=0;
    double wmin=0;
    double wmax=0;
};

struct RegionRect {

    double x1=0;
    double y1=0;
    double x2=0;
    double y2=0;
    double width=0;
    double theta=0;
    double dx=0;
    double dy=0;
    double prec=0;
    double p=0;

    RegionRect() = default;
    explicit RegionRect(const std::vector<RegionPoint> &region_points, double reg_angle, double prec, double p);
};

Centroid getRegCentroid(const std::vector<RegionPoint> &region_points);
double getRegAngle(const std::vector<RegionPoint> &region_points, const Centroid &centroid, double reg_angle, double prec);
InertiaMatrix getInertiaMatrix(const std::vector<RegionPoint> &regionPoints, const Centroid &centroid);
double calculateEigenValue(const InertiaMatrix &inertiaMatrix);
RegionSize getRegionSize(const std::vector<RegionPoint> &regionPoints, const Centroid &centroid, double dx, double dy);
double getRegionDensity(const RegionRect &regionRect, size_t num_points);



#endif // TEST_REGIONRECT_H
