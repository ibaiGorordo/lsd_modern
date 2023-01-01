//
// Created by ibaig on 1/1/2023.
//

#ifndef TEST_GRADIENTCALCULATOR_H
#define TEST_GRADIENTCALCULATOR_H


#include <vector>

class GradientCalculator {

public:
    void calculateGradients(const unsigned char *image,
                            int width, int height,
                            double threshold,
                            std::vector<double> &magnitudes,
                            std::vector<double> &angles);

private:
    constexpr static double not_defined = 1024.0;
private:

};


#endif //TEST_GRADIENTCALCULATOR_H
