//
// Created by ibaig on 1/1/2023.
//

#include "cmath"
#include "algorithm"
#include "GradientCalculator.h"

void GradientCalculator::calculateGradients(const unsigned char *image,
                                            int width, int height,
                                            double *grad_x,
                                            double *grad_y,
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
            const auto dx = DA + BC; // We don't divide by 2 because we will do dx/dy
            const auto dy = DA - BC;
            grad_x[index] = dx;
            grad_y[index] = dy;
            const auto norm = dx * dx + dy * dy;
            bad_pixels[index] = norm <= threshold_squared;
            magnitudes[index] = std::sqrt(norm)/2.0;
        }
    }
}
