//
// Created by ibaig on 12/30/2022.
//

#ifndef TEST_GAUSSIANDOWNSAMPLER_H
#define TEST_GAUSSIANDOWNSAMPLER_H


#include <vector>

class GaussianDownsampler {
public:
    explicit GaussianDownsampler(float scale=0.8, float sigma_scale=0.6);
    void blur(const unsigned  char *in_img, unsigned  char *out_img, int width, int height);
    void blur_downsample(const unsigned  char *in_img, unsigned  char *out_img, int width, int height);

    float scale;
private:
    float sigma;
    std::vector<float> kernel;
    unsigned char *smoothed_img;

private:
    static std::vector<float> get_gaussian_kernel(float sigma);
    void create_smoothed_img(int width, int height);
};


#endif //TEST_GAUSSIANDOWNSAMPLER_H
