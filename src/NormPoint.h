//
// Created by Ibai Gorordo on 2023/03/05.
//

#ifndef TEST_NORMPOINT_H
#define TEST_NORMPOINT_H

struct NormPoint {
    int x{};
    int y{};
    unsigned short quant_norm{};

    explicit NormPoint(int x, int y, unsigned short quant_norm) : x(x), y(y), quant_norm(quant_norm) {}

    bool operator > (const NormPoint &other) const
    {
        return quant_norm > other.quant_norm;
    }
};


#endif//TEST_NORMPOINT_H
