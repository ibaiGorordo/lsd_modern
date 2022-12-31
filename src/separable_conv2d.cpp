//
// Created by ibaig on 12/30/2022.
//

#include "separable_conv2d.h"
#include "vector"

void copy_sum(float *sum, unsigned char *out,
              int row, int width,
              int kernel_center) {
    auto row_pixels = row * width;
    for (int x = kernel_center; x < width-kernel_center; ++x)
    {
        out[row_pixels + x] = static_cast<unsigned char>(sum[x]+ 0.5f);
        sum[x] = 0;
    }
}

void vertical_conv(const float *tmp, float *sum,
                   const std::vector<float>& kernel,
                   int k, int kernel_center,
                   int row, int width)
{
    auto row_pixels = (k-kernel_center+row) * width;
    for (int x = kernel_center; x < width-kernel_center; ++x)
    {
        sum[x] += tmp[row_pixels + x] * kernel[k];
    }
}

//TODO: Since the kernel is symmetrical, we can reduce the multiplications by ~half
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

    // Copy the contents of the input image to the output image
    // This is done to avoid the border effects
    std::copy(in, in + width * height, out);

    for (int y = 0; y < height; ++y) {
        auto row_pixels = y * width;

#pragma omp parallel for reduction(+:temp_sum)
        // [kernel_center, width - kernel_center - 1]
        for (int x = kernel_center; x < end_index; ++x) {
            int idx = row_pixels + x;
            auto offset_idx = idx - kernel_center;
            auto temp_sum = 0.0f;
            for (int k = 0; k < kernel_size; ++k)
                temp_sum += static_cast<float>(in[offset_idx + k]) * kernel[k];
            tmp[idx] = temp_sum;
        }
    }
    end_index = height - kernel_center;

    // [kCenter, height - kCenter - 1]
    for (int y = kernel_center; y < end_index; ++y)
    {
        for (int k = 0; k < kernel_size; ++k)
        {
            vertical_conv(tmp, sum, kernel, k, kernel_center, y, width);
        }
        copy_sum(sum, out, y, width, kernel_center);
    }

    delete[] tmp;
    delete[] sum;
}
