//
// Created by ibaig on 12/30/2022.
//
#include <numeric>
#include "cmath"
#include "algorithm"
#include "GaussianDownsampler.h"
#include "separable_conv2d.h"

using Kernel = std::vector<float>;

GaussianDownsampler::GaussianDownsampler(float scale,
                                         float sigma_scale) {
    this->scale = scale;
    this->kernel = get_gaussian_kernel(scale, sigma_scale);
}

void GaussianDownsampler::blur_downsample(const unsigned char *in_img,
                                          unsigned char *out_img,
                                          int width, int height) {
    check_new_img_size(width, height);
    blur(in_img, smoothed_img.data(), width, height);

    bilinear_resize(smoothed_img.data(), out_img);
}

void GaussianDownsampler::blur(const unsigned  char *in_img,
                               unsigned  char *out_img,
                               int width, int height) {
    separable_conv2d(in_img, out_img, width, height, kernel);
}

void GaussianDownsampler::bilinear_resize(const unsigned char* input, unsigned char* output) const
{
#pragma omp parallel for collapse(2)
    auto output_idx = 0;
    for (int y = 0; y < resize_height; y++)
    {
        const auto y0 = y_values[y];
        auto y1 = y0 + 1;
        const auto fracy = y_fracs[y];

        if (y1 >= img_height)
        {
            y1 = y0;
        }

        const auto row_pixels = y0 * img_width;
        const auto row_pixels_next = y1 * img_width;

        for (int x = 0; x < resize_width; x++)
        {
            const auto x0 = x_values[x];
            auto x1 = x0 + 1;
            const auto fracx = x_fracs[x];

            if (x1 >= img_width)
            {
                x1 = x0;
            }

            const auto val1 = input[row_pixels + x0];
            const auto val2 = input[row_pixels + x1];
            const auto val3 = input[row_pixels_next + x0];
            const auto val4 = input[row_pixels_next + x1];

            output[output_idx++] = static_cast<unsigned char>((val1 * (int_factor - fracx) * (int_factor - fracy) +
                val2 * fracx * (int_factor - fracy) +
                val3 * (int_factor - fracx) * fracy +
                val4 * fracx * fracy) >> (2 * factor_shift));

        }
    }
}


void GaussianDownsampler::check_new_img_size(int width, int height) {

    // Check smooth image in case it is empty
    check_smoothed_img(width, height);

    if(img_width == width && img_height == height){
        return;
    }

    img_width = width;
    img_height = height;
    resize_width = static_cast<int>(std::ceil(static_cast<float>(width) * scale));
    resize_height = static_cast<int>(std::ceil(static_cast<float>(height) * scale));
    calculate_x_values();
    calculate_y_values();
}


void GaussianDownsampler::check_smoothed_img(int width, int height) {
    if(!smoothed_img.empty())
    {
        if(img_width == width && img_height == height)
            return;
        else
        {
            smoothed_img.clear();
        }
    }
    smoothed_img.resize(width * height);
}

std::vector<float> GaussianDownsampler::get_gaussian_kernel(float scale, float sigma_scale) {
    const auto sigma = scale < 1.0 ? sigma_scale / scale : sigma_scale;
    const auto sprec = 3.0;
    const auto h =  static_cast<int>(std::ceil(sigma * std::sqrt(2 * sprec * std::log(10.0))));
    auto kernel_size = 1 + 2 * h;

#pragma omp parallel for
    Kernel kernel(kernel_size);
    for (int i = 0; i < kernel_size; ++i) {
        kernel[i] = static_cast<float>(std::exp(-0.5 * std::pow(static_cast<float>(i - h) / sigma, 2)));
    }

    auto sum = std::reduce(kernel.begin(), kernel.end());
    std::transform(kernel.begin(), kernel.end(), kernel.begin(),
                   [sum](float x) { return x / sum; });

    return kernel;
}

void GaussianDownsampler::calculate_x_values() {
    if(!x_values.empty() && x_values.size() == resize_width)
        return;

    x_values.resize(resize_width);
    x_fracs.resize(resize_width);

    const auto xs = std::lround(static_cast<double>(int_factor) * img_width / resize_width);

#pragma omp parallel for
    for (int x = 0; x < resize_width; x++) {
        const auto sx = x * xs;
        x_values[x] = sx >> factor_shift;
        x_fracs[x] = sx - (x_values[x]  << factor_shift);
    }

}

void GaussianDownsampler::calculate_y_values() {
    if(!y_values.empty() && y_values.size() == resize_height)
        return;

    y_values.resize(resize_height);
    y_fracs.resize(resize_height);

    const auto ys = std::lround(static_cast<double>(int_factor) * img_height / resize_height);

#pragma omp parallel for
    for (int y = 0; y < resize_height; y++) {
        const auto sy = y * ys;
        y_values[y] = sy >> factor_shift;
        y_fracs[y] = sy - (y_values[y]  << factor_shift);
    }

}
