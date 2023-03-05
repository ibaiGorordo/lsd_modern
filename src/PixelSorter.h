//
// Created by Ibai Gorordo on 2023/03/05.
//

#ifndef TEST_PIXELSORTER_H
#define TEST_PIXELSORTER_H


#include "NormPoint.h"
#include <vector>

class PixelSorter {

public:

    static void sort(std::vector<NormPoint>& sorted_pixels,
                                const double *magnitudesPtr,
                                const unsigned char *usedPixelsPtr,
                                int imgWidth, int imgHeight);
private:
    static constexpr uint16_t numBins = 1024*52; // 52 ~= 256/5.22 (gradient threshold)
    static constexpr uint16_t maxGrad = 256;
    static constexpr double quantCoeff = (double) numBins / maxGrad;

};


#endif//TEST_PIXELSORTER_H
