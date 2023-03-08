//
// Created by Ibai Gorordo on 2023/03/07.
//

#ifndef TEST_RECTIMPROVER_H
#define TEST_RECTIMPROVER_H


#include <vector>

class RectImprover {


public:
    RectImprover(double p, double logNt);


private:

    static constexpr int maxPopulation = 1024;
    static constexpr int numImprovements = 5;
    bool nfaTable[maxPopulation*(numImprovements+1)]{};
    double invTable[maxPopulation]{};   /* table to keep computed inverse values */

private:
    void computeNfaTable(double pInit, double logNt);
    double nfa(int totalCount, int alignedCount, double p, double logNT);
    static constexpr double logGammaWindschitl(double x);
    static constexpr double logGammaLanczos(double x);
    static constexpr bool goodNfa(double nfa, double eps);
    static constexpr double logGamma(double x);
};


#endif//TEST_RECTIMPROVER_H
