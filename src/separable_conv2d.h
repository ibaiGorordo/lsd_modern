//
// Created by ibaig on 12/30/2022.
//

#ifndef TEST_SEPARABLE_CONV2D_H
#define TEST_SEPARABLE_CONV2D_H


#include <vector>

void separable_conv2d(const unsigned char* in, unsigned char* out,
                      int width, int height,
                      const std::vector<float>& kernel);

#endif //TEST_SEPARABLE_CONV2D_H
