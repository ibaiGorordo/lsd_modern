//
// Created by ibaig on 1/1/2023.
//

#ifndef TEST_GRADIENTCALCULATOR_H
#define TEST_GRADIENTCALCULATOR_H


#include <vector>

class GradientCalculator {

public:
    GradientCalculator() = default;
    explicit GradientCalculator(double threshold) : threshold(threshold) {threshold_squared = threshold * threshold *4;}
    void calculateGradients(const unsigned char *image,
                            int width, int height,
                            double *grad_x,
                            double *grad_y,
                            double *magnitudes,
                            unsigned char *bad_pixels) const;

private:
    double threshold;
    double threshold_squared;
private:

};


#endif //TEST_GRADIENTCALCULATOR_H
