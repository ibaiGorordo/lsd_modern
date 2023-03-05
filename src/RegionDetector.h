//
// Created by ibaig on 1/3/2023.
//

#ifndef TEST_REGIONDETECTOR_H
#define TEST_REGIONDETECTOR_H

#include <vector>

#include "NormPoint.h"
#include "RegionPoint.h"
#include "RegionRect.h"


class RegionDetector {

public:
    RegionDetector() = default;
    explicit RegionDetector(double threshold=22.5, double density_threshold=0.7);

    void detect(const double *angles,
                const double *magnitudes,
                unsigned char *bad_pixels,
                int width, int height);

private:
    int minRegSize{};
    int imgWidth = 0;
    int imgHeight = 0;

    double angTh{};
    double angThNorm{};

    static constexpr double refineCoeffSq = 0.75*0.75;    double densityTh{};

    std::vector<NormPoint> sorted_pixels;
    std::vector<RegionPoint> region_points;
    double regDx{};
    double regDy{};
    double regAngle{};
    double regDensity{};
    bool regionFound{};
    int regionSize{};
    RegionRect regionRect;
    int regionCount = 0;

    const double *anglesPtr{};
    const double *magnitudesPtr{};
    unsigned char *usedPixelsPtr{};


private:
    void searchRegions();
    void findRegion(int x, int y, double angleThreshold, int min_size);
    void refineRegion();
    void regionGrow(int x, int y, double angle_threrehold);
    void calculateRect();
    void checkNewImgSize(int width, int height);
    void resetRegion();
    void registerPoint(int x, int y);
    double calculateRegionStdAngle();
    void resetPoint(RegionPoint &point);

    static bool isAligned(double angle, double reg_angle, double tan_th);
    static constexpr int minLimit(int x) { return x == 0 ? 0 : x - 1; }
    static constexpr int maxLimit(int x, int max) { return x == max-1 ? max-1 : x + 1; }


};


#endif //TEST_REGIONDETECTOR_H
