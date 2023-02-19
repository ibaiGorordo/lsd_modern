//
// Created by ibaig on 1/3/2023.
//

#ifndef TEST_REGIONDETECTOR_H
#define TEST_REGIONDETECTOR_H

#include <vector>

#include "RegionPoint.h"
#include "RegionRect.h"

struct NormPoint {
    int x;
    int y;
    uint16_t quant_norm{};

    explicit NormPoint(int x, int y, unsigned int quant_norm) : x(x), y(y), quant_norm(quant_norm) {}

    bool operator > (const NormPoint &other) const
    {
        return quant_norm > other.quant_norm;
    }
};


class RegionDetector {

public:
    RegionDetector() = default;
    explicit RegionDetector(double threshold=22.5, double density_threshold=0.7);

    void detect(const double *angles,
                const double *magnitudes,
                unsigned char *bad_pixels,
                int width, int height);

private:
    int min_reg_size{};
    int img_width = 0;
    int img_height = 0;

    double ang_th{};
    double ang_th_norm{};
    double density_th{};

    static constexpr uint16_t num_bins = 1024*52; // 52 ~= 256/5.22 (gradient threshold)
    static constexpr uint16_t max_grad = 256;
    static constexpr double quant_coeff = (double) num_bins / max_grad;
    std::vector<NormPoint> sorted_pixels;

    std::vector<RegionPoint> region_points;
    double reg_dx{};
    double reg_dy{};
    double reg_angle{};
    double reg_density{};
    RegionRect region_rect;
    int region_count = 0;

    const double *angles_ptr{};
    const double *magnitudes_ptr{};
    unsigned char *used_pixels_ptr{};


private:
    void search_regions();
    void region_grow(int x, int y);
    void get_sorted_pixels();
    void check_new_img_size(int width, int height);
    void reset_region();
    void register_point(int x, int y);
    void refine_region();

    static bool is_aligned(double angle, double reg_angle, double tan_th);
    static constexpr int min_limit(int x) { return x == 0 ? 0 : x - 1; }
    static constexpr int max_limit(int x, int max) { return x == max-1 ? max-1 : x + 1; }

};


#endif //TEST_REGIONDETECTOR_H
