//
// Created by ibaig on 12/30/2022.
//

#ifndef TEST_GAUSSIANDOWNSAMPLER_H
#define TEST_GAUSSIANDOWNSAMPLER_H


#include <vector>

class GaussianDownsampler {
public:
    explicit GaussianDownsampler(double scale=0.8, double sigma_scale=0.6);
    double *downsample(double *img);

    static std::vector<std::vector<double>> calculate_kernel(double scale, double sigma_scale);

private:
    double scale;
    double sigma_scale;
    std::vector<std::vector<double>> kernel;
    double *smoothed_img{};

private:

};


#endif //TEST_GAUSSIANDOWNSAMPLER_H
