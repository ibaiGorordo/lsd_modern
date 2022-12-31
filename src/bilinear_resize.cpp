#include <algorithm>
#include <cmath>
#include <vector>

#include "bilinear_resize.h"
void bilinear_resize(const unsigned char* input, int inputWidth, int inputHeight,
                      unsigned char* output, int outputWidth, int outputHeight)
{
    float xs = (float)inputWidth / outputWidth;
    float ys = (float)inputHeight / outputHeight;

    float fracx, fracy, ifracx, ifracy, sx, sy, l0, l1, rf, gf, bf;
    int x0, x1, y0, y1;
    unsigned char c1, c2, c3, c4, a;

    int srcIdx = 0;

    for (int y = 0; y < outputHeight; y++)
    {
        for (int x = 0; x < outputWidth; x++)
        {
            sx = x * xs;
            sy = y * ys;
            x0 = (int)sx;
            y0 = (int)sy;

            // Calculate coordinates of the 4 interpolation points
            fracx = sx - x0;
            fracy = sy - y0;
            ifracx = 1.0f - fracx;
            ifracy = 1.0f - fracy;
            x1 = x0 + 1;
            if (x1 >= inputWidth)
            {
                x1 = x0;
            }
            y1 = y0 + 1;
            if (y1 >= inputHeight)
            {
                y1 = y0;
            }

            // Read source color
            c1 = input[y0 * inputWidth + x0];
            c2 = input[y0 * inputWidth + x1];
            c3 = input[y1 * inputWidth + x0];
            c4 = input[y1 * inputWidth + x1];

            // Calculate colors
            // Alpha
            l0 = ifracx * c1 + fracx * c2;
            l1 = ifracx * c3 + fracx * c4;
            output[srcIdx++]  = (unsigned char )(ifracy * l0 + fracy * l1);
        }
    }
}