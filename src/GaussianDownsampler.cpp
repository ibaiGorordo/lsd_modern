//
// Created by ibaig on 12/30/2022.
//
#include "GaussianDownsampler.h"
#include "fast_gaussian_blur.h"

using Kernel = std::vector<std::vector<float>>;

GaussianDownsampler::GaussianDownsampler(float scale,
                                         float sigma_scale) {
    this->scale = scale;
    this->sigma_scale = sigma_scale;
    this->sigma = scale < 1.0 ? sigma_scale / scale : sigma_scale;
}

void GaussianDownsampler::downsample(float *in_img, float *out_img, int width, int height) {

    fast_gaussian_blur(in_img, out_img, width, height, sigma);
}


