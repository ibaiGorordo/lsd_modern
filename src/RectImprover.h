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
    bool nfaTable[maxPopulation*(numImprovements+1)];

private:
    void computeNfaTable(double pInit, double logNt);

    static bool nfa(int totalCount, int alignedCount, double p, double logNT, double eps);
};


#endif//TEST_RECTIMPROVER_H
