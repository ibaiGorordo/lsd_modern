//
// Created by ibaig on 12/30/2022.
//
#include <numbers>
#include <algorithm>
#include <cmath>

#include "GaussianDownsampler.h"

using Kernel = std::vector<std::vector<double>>;

GaussianDownsampler::GaussianDownsampler(double scale,
                                         double sigma_scale) {
    this->scale = scale;
    this->sigma_scale = sigma_scale;
    this->kernel = calculate_kernel(scale, sigma_scale);
}

double *GaussianDownsampler::downsample(double *img) {



    return smoothed_img;
}

Kernel GaussianDownsampler::calculate_kernel(double scale,
                                             double sigma_scale) {
    auto sigma = scale < 1.0 ? sigma_scale / scale : sigma_scale;
    const double sprec = 3;
    const auto h =  (int)(ceil(sigma * sqrt(2 * sprec * log(10.0))));
    auto kernel_size = 1 + 2 * h;
    int center = (kernel_size - 1) / 2;
    double s = 2 * sigma * sigma;

    Kernel kernel(kernel_size, std::vector<double>(kernel_size, 0.0));
    double sum = 0.0;
    for (int i = 0; i < kernel_size; ++i) {
        for (int j = 0; j < kernel_size; ++j) {
            double x = i - center;
            double y = j - center;
            double r = std::sqrt(x * x + y * y);
            double val = std::exp(-(r * r) / s) / (std::numbers::pi * s);
            kernel[i][j] = val;
            sum += val;
        }
    }
    for (auto& row : kernel) {
        for (auto& val : row) {
            val /= sum;
        }
    }

    return kernel;
}


