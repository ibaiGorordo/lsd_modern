//
// Created by ibaig on 12/30/2022.
//
#include "cmath"
#include "algorithm"
#include "GaussianDownsampler.h"
#include "separable_conv2d.h"

using Kernel = std::vector<float>;

GaussianDownsampler::GaussianDownsampler(float scale,
                                         float sigma_scale) {
    this->scale = scale;
    this->sigma = scale < 1.0 ? sigma_scale / scale : sigma_scale;
    this->kernel = get_gaussian_kernel(sigma);
}

void GaussianDownsampler::downsample(const unsigned  char *in_img,
                                     unsigned  char *out_img,
                                     int width, int height) {
    separable_conv2d(in_img, out_img, width, height, kernel);
}

std::vector<float> GaussianDownsampler::get_gaussian_kernel(float sigma) {
    const double sprec = 3;
    const auto h =  (int)(std::ceil(sigma * std::sqrt(2 * sprec * std::log(10.0))));
    auto kernel_size = 1 + 2 * h;

    Kernel kernel(kernel_size);
    auto sum = 0.0;
    for (int i = 0; i < kernel_size; ++i) {
        kernel[i] = std::exp(-0.5 * std::pow((i - h) / sigma, 2));
        sum += kernel[i];
    }

    for(auto &v : kernel)
        v /= sum;

    return kernel;
}


