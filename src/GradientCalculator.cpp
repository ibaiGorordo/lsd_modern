//
// Created by ibaig on 1/1/2023.
//

#include "algorithm"
#include <cmath>

#include "utils.h"
#include "GradientCalculator.h"


void GradientCalculator::calculateGradients(const unsigned char *image,
                                            int width, int height,
                                            double *angles,
                                            double *magnitudes,
                                            unsigned char *bad_pixels) const {

#pragma omp parallel for collapse(2)
    // Compute the gradient on the image
    for (int y = 0; y < height - 1; y++) {
        auto row_offset = y * width;
        for (int x = 0; x < width - 1; x++) {
            const auto index = row_offset + x;
            const auto DA = image[index + width + 1] - image[index];
            const auto BC = image[index + 1] - image[index + width];
            const auto dx = DA + BC;
            const auto dy = DA - BC;
            const auto norm = dx * dx + dy * dy;
            const auto is_bad = norm <= threshold_squared;
            if (is_bad) {
                bad_pixels[index] = is_bad;
                continue;
            }
            bad_pixels[index] = is_bad;
            magnitudes[index] = std::sqrt(norm)/2.0;
            const auto dx_float = static_cast<double >(dx);
            const auto dy_float = static_cast<double >(dy);

            angles[index] = fast_atan2d(dx_float, -dy_float);

        }
    }
}
