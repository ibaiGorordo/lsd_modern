//
// Created by ibaig on 12/30/2022.
//

#ifndef TEST_GAUSSIANDOWNSAMPLER_H
#define TEST_GAUSSIANDOWNSAMPLER_H


#include <vector>

class GaussianDownsampler {
public:
    explicit GaussianDownsampler(float scale=0.8f, float sigma_scale=0.6f);

    void blur_downsample(const unsigned  char *in_img,
                         unsigned  char *out_img,
                         int width, int height);

    void blur(const unsigned  char *in_img,
              unsigned  char *out_img,
              int width, int height);

    float scale;
private:
    std::vector<float> kernel;

    std::vector<float> tmp_img;
    std::vector<float> row_sum;
    std::vector<unsigned char> smoothed_img;
    int img_width{};
    int img_height{};
    int resize_width{};
    int resize_height{};

    std::vector<int> x_values;
    std::vector<int> x_fracs;
    std::vector<int> y_values;
    std::vector<int> y_fracs;

    static constexpr int int_factor = 2048;
    static constexpr int factor_shift = 11;

private:
    void bilinear_resize(const unsigned char* input, unsigned char* output) const;
    void separable_conv2d(const unsigned char* in, unsigned char* out);


    void check_new_img_size(int width, int height);
    static std::vector<float> get_gaussian_kernel(float scale, float sigma_scale);
    void check_smoothed_img(int width, int height);
    void calculate_x_values();
    void calculate_y_values();
    static void copy_sum(const std::vector<float>& sum,
                         unsigned char *out,
                         int row, int width,
                         int kernel_center);
    static void vertical_conv(const std::vector<float>& tmp,
                              std::vector<float>& sum,
                              const std::vector<float>& kernel,
                              int k, int kernel_center,
                              int row, int width);
};


#endif //TEST_GAUSSIANDOWNSAMPLER_H
