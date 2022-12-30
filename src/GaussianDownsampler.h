//
// Created by ibaig on 12/30/2022.
//

#ifndef TEST_GAUSSIANDOWNSAMPLER_H
#define TEST_GAUSSIANDOWNSAMPLER_H


#include <vector>

class GaussianDownsampler {
public:
    explicit GaussianDownsampler(float scale=0.8, float sigma_scale=0.6);
    void downsample(float *in_img, float *out_img, int width, int height);

private:
    float scale;
    float sigma_scale;
    float sigma;

private:

};


#endif //TEST_GAUSSIANDOWNSAMPLER_H
