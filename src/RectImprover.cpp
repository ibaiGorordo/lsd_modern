//
// Created by Ibai Gorordo on 2023/03/07.
//

#include "RectImprover.h"

bool RectImprover::nfa(int totalCount, int alignedCount, double p, double logNT, double eps) {
    if (alignedCount == 0) return false;
    if (alignedCount == totalCount) return true;
    double log1term = logGamma(totalCount + 1) - logGamma(alignedCount + 1) - logGamma(totalCount - alignedCount + 1);
    double term = exp(log1term + alignedCount * log(p) + (totalCount - alignedCount) * log(1.0 - p));
    if (term > eps) return true;
    double binTail = term;
    int i = alignedCount + 1;
    while (i <= totalCount) {
        double binCoef = log1term + log(i) - log(totalCount - i + 1);
        binTail *= p / (1.0 - p) * (totalCount - i + 1) / i;
        if (binCoef + log(binTail) < logNT - logEPS) return false;
        if (binCoef + log(binTail) < logNT + logEPS) return true;
        i++;
    }
    return true;


}

void RectImprover::computeNfaTable(double pInit, double logNt) {
    double p = pInit;
    int index = 0;
    for (int k = 0; k <= numImprovements; k++) {
        for (int j = 0; j < maxPopulation; j++) {
            for (int i = 0; i < maxPopulation; i++) {
                // TODO: Check if once the value becomes false/true, it will always be false/true
                auto good_nfa = nfa(i, j, p, logNt, 1.0);
                printf("Computing NFA table: %d/%d: %d\n", i,j,good_nfa);
                //Checl when the value becomes false, store the change position and stop
//                nfaTable[index] = nfa(i, j, p, logNt, 1.0);
                index++;
            }
        }
        p/=2;
    }
}
