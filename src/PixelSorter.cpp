//
// Created by Ibai Gorordo on 2023/03/05.
//

#include "PixelSorter.h"

void PixelSorter::sort(std::vector<NormPoint>& sorted_pixels,
                                  const double *magnitudesPtr,
                                  const unsigned char *usedPixelsPtr,
                                  int imgWidth, int imgHeight)
{
    sorted_pixels.clear();
    for(int y = 0; y < imgHeight-1; y++)
    {
        auto row_start = y * imgWidth;
        for(int x = 0; x < imgWidth-1; x++)
        {
            int index = row_start + x;
            if(!usedPixelsPtr[index])
            {
                auto quant_norm = static_cast<uint16_t>(magnitudesPtr[index] * quantCoeff +0.5);
                if (quant_norm == 0)
                    continue;
                sorted_pixels.emplace_back(x, y, quant_norm);
            }
        }
    }

    // Sort in descending order
    std::sort(sorted_pixels.begin(), sorted_pixels.end(), std::greater<>());
    printf("First point: %d %d\n", sorted_pixels[0].x, sorted_pixels[0].y);
}
