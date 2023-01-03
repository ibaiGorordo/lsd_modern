//
// Created by ibaig on 1/3/2023.
//
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "RegionDetector.h"

RegionDetector::RegionDetector(double threshold) {
    this->ang_th = threshold;
    p = threshold / 180.0;
}

void RegionDetector::detect(const double *grad_x, const double *grad_y,
                            const double *magnitudes, const unsigned char *bad_pixels,
                            int width, int height) {
    check_new_img_size(width, height);

    // Get the sorted pixel coordinates bsaed on the magnitude that are not bad pixels
    get_sorted_pixels(magnitudes, bad_pixels);

    // Search regions
    search_regions(grad_x, grad_y, magnitudes, bad_pixels);

}

void RegionDetector::check_new_img_size(int width, int height) {
    if (img_width != width || img_height != height) {
        auto log_nt = 5 * (std::log10(double(img_width)) + std::log10(double(img_height))) / 2 + std::log10(11.0);
        min_reg_size = -log_nt / log10(p);
        img_width = width;
        img_height = height;
    }
}


void RegionDetector::get_sorted_pixels(const double *magnitudes,
                                  const unsigned char *bad_pixels)
{
    sorted_pixels.clear();
#pragma omp parallel for schedule(dynamic)
    for(int y = 0; y < img_height; y++)
    {
        auto row_start = y * img_width;
        for(int x = 0; x < img_width; x++)
        {
            int index = row_start + x;
            if(!bad_pixels[index])
            {
                auto quant_norm = static_cast<uint16_t>(magnitudes[index] * quant_coeff +0.5);
                sorted_pixels.emplace_back(x, y, quant_norm);
            }
        }
    }

    // Sort in descending order
    std::sort(sorted_pixels.begin(), sorted_pixels.end(), std::greater<>());
}

void RegionDetector::search_regions(const double *grad_x, const double *grad_y,
                                    const double *magnitudes,
                                    const unsigned char *bad_pixels) {
    for(auto& point : sorted_pixels)
    {

    }


}




