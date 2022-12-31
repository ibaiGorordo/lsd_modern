//
// Created by ibaig on 12/30/2022.
//

#include "separable_conv2d.h"
#include "vector"

void copy_sum(float *sum, unsigned char *out, int row, int width) {
    auto row_pixels = row * width;
    for (int i = 0; i < width; ++i)
    {
        out[row_pixels + i] = static_cast<unsigned char>(sum[i]+ 0.5f);
        sum[i] = 0;
    }
}

void vertical_conv(const float *tmp, float *sum,
                   const std::vector<float>& kernel,
                   int k, int kernel_center,
                   int row, int width)
{
    auto row_pixels = (k-kernel_center+row) * width;
    for (int i = 0; i < width; ++i)
    {
        sum[i] += tmp[row_pixels + i] * kernel[k];
    }
}

void separable_conv2d(const unsigned char* in, unsigned char* out,
                      int width, int height,
                      const std::vector<float>& kernel)
{
    int kernel_size = static_cast<int>(kernel.size());
    auto kernel_center = kernel_size >> 1;
    auto side = kernel_size - kernel_center - 1;
    auto end_index = width - kernel_center;

    auto *tmp = new float[width * height];
    auto *sum = new float[width];

    for (int j = 0; j < height; ++j)
    {
        auto row_pixels = j * width;

#pragma omp parallel for reduction(+:temp_sum)
        // [0, kernel_center - 1]
        for (int i = 0; i < kernel_center; ++i)
        {
            int idx = row_pixels + i;
            auto offset_idx = idx - kernel_center;
            auto temp_sum = 0.0f;
            for (int k = kernel_center - i; k < kernel_size; ++k)
                temp_sum += static_cast<float>(in[offset_idx + k]) * kernel[k];
            tmp[idx] = temp_sum;
        }

#pragma omp parallel for reduction(+:temp_sum)
        // [kernel_center, width - kernel_center - 1]
        for (int i = kernel_center; i < end_index; ++i)
        {
            int idx = row_pixels + i;
            auto offset_idx = idx - kernel_center;
            auto temp_sum = 0.0f;
            for (int k = 0; k < kernel_size; ++k)
                temp_sum += static_cast<float>(in[offset_idx + k]) * kernel[k];
            tmp[idx] = temp_sum;
        }

#pragma omp parallel for reduction(+:temp_sum)
        // [width - kernel_center, width - 1]
        for (int i = end_index; i < width; ++i)
        {
            int idx = row_pixels + i;
            auto offset_idx = idx - kernel_center;
            auto temp_sum = 0.0f;
            for (int k = 0; k < kernel_size - (i - end_index); ++k)
                temp_sum += static_cast<float>(in[offset_idx + k]) * kernel[k];
            tmp[idx] = temp_sum;
        }
    }

    end_index = height - kernel_center;
    // [0, kCenter - 1]
    for (int j = 0; j < kernel_center; ++j)
    {
        for (int k = kernel_center - j; k < kernel_size; ++k)
        {
            vertical_conv(tmp, sum, kernel, k, kernel_center, j, width);
        }
        copy_sum(sum, out, j, width);
    }

    // [kCenter, height - kCenter - 1]
    for (int j = kernel_center; j < end_index; ++j)
    {
        for (int k = 0; k < kernel_size; ++k)
        {
            vertical_conv(tmp, sum, kernel, k, kernel_center, j, width);
        }
        copy_sum(sum, out, j, width);
    }

    // [endIndex, dataSizeY - 1]
    for (int j = end_index; j < height; ++j)
    {
        for (int k = 0; k < kernel_size - (j - end_index + 1); ++k)
        {
            vertical_conv(tmp, sum, kernel, k, kernel_center, j, width);
        }
        copy_sum(sum, out, j, width);
    }
}
