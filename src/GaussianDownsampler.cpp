//
// Created by ibaig on 12/30/2022.
//
#include "cmath"
#include "algorithm"
#include "GaussianDownsampler.h"
#include "separable_conv2d.h"
#include "bilinear_resize.h"

using Kernel = std::vector<float>;

GaussianDownsampler::GaussianDownsampler(float scale,
                                         float sigma_scale) {
    this->scale = scale;
    this->sigma = scale < 1.0 ? sigma_scale / scale : sigma_scale;
    this->kernel = get_gaussian_kernel(sigma);
}

void GaussianDownsampler::blur(const unsigned  char *in_img,
                               unsigned  char *out_img,
                               int width, int height) {
    separable_conv2d(in_img, out_img, width, height, kernel);
}

void GaussianDownsampler::blur_downsample(const unsigned char *in_img, unsigned char *out_img, int width, int height) {
    create_smoothed_img(width, height);
    separable_conv2d(in_img, smoothed_img, width, height, kernel);

    auto new_width = static_cast<int>(std::ceil(width * scale));
    auto new_height = static_cast<int>(std::ceil(height * scale));
    bilinear_resize(smoothed_img, width, height, out_img, new_width, new_height);
}



std::vector<float> GaussianDownsampler::get_gaussian_kernel(float sigma) {
    const double sprec = 3;
    const auto h =  (int)(std::ceil(sigma * std::sqrt(2 * sprec * std::log(10.0))));
    auto kernel_size = 1 + 2 * h;

    Kernel kernel(kernel_size);
    auto sum = 0.0;
    for (int i = 0; i < kernel_size; ++i) {
        kernel[i] = static_cast<float>(std::exp(-0.5 * std::pow((i - h) / sigma, 2)));
        sum += kernel[i];
    }

    for(auto &v : kernel)
        v /= sum;

    return kernel;
}

void GaussianDownsampler::create_smoothed_img(int width, int height) {
    if(smoothed_img != nullptr)
    {
        if(smoothed_img[0] == width && smoothed_img[1] == height)
            return;
        delete[] smoothed_img;
    }

    smoothed_img = new unsigned char[width * height];
}


