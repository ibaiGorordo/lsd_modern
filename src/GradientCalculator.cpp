//
// Created by ibaig on 1/1/2023.
//

#include "cmath"
#include "algorithm"
#include "GradientCalculator.h"

void GradientCalculator::calculateGradients(const unsigned char *image,
                                            int width, int height,
                                            double threshold,
                                            double* magnitudes,
                                            double* angles) {

    std::fill(magnitudes, magnitudes + width * height, 0.0);
    std::fill(angles, angles + width * height, not_defined);

    // Compute the gradient on the image
    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            int index = y * width + x;
            double DA = image[index + width + 1] - image[index];
            double BC = image[index + 1] - image[index + width];
            double dx = DA + BC;
            double dy = DA - BC;
            double magnitude = std::sqrt((dx * dx + dy * dy) / 4.0);

            magnitudes[index] = magnitude;
            if(magnitude < threshold)
            {
                angles[index] = not_defined;
            }
            else
            {
                angles[index] = std::atan2(dy, dx);
            }
        }
    }
}
