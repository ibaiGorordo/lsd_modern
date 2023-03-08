//
// Created by Ibai Gorordo on 2023/03/07.
//

#include "RectImprover.h"
#include <cmath>

constexpr double kLanczosCoefficients[] = { 75122.6331530, 80916.6278952, 36308.2951477,
                                                  8687.24529705, 1168.92649479, 83.8676043424,
                                                  2.50662827511 };

RectImprover::RectImprover(double p, double logNt) {
    computeNfaTable(p, logNt);
}

double RectImprover::nfa(int totalCount, int alignedCount, double p, double logNT) {
    if (alignedCount == 0) return -logNT;
    if (alignedCount == totalCount) return -logNT - (double) totalCount * std::log10(p);
    auto p_term = p / (1.0-p);

    auto log1term = logGamma( (double) totalCount + 1.0 ) - logGamma( (double) alignedCount + 1.0 )
               - logGamma( (double) (totalCount-alignedCount) + 1.0 )
               + (double) alignedCount * log(p) + (double) (totalCount-alignedCount) * log(1.0-p);

    // large negative log1term, will mean term is close to 0
    if(log1term < 10.0)
    {
        if( (double) alignedCount > (double) totalCount * p )     /* at begin or end of the tail?  */
            return -log1term / M_LN10 - logNT;   /* end: use just the first term  */
        else
            return -logNT;
    }

    auto term = std::exp(log1term);
    double tolerance = 0.1;
    auto bin_tail = term;
    for(int i=alignedCount+1; i<=totalCount; i++)
    {
        auto bin_term = (double) (totalCount-i+1) * ( i<maxPopulation ?
                                                       ( invTable[i]!=0.0 ? invTable[i] : ( invTable[i] = 1.0 / (double) i ) ) :
                                                       1.0 / (double) i );

        auto mult_term = bin_term * p_term;
        term *= mult_term;
        bin_tail += term;
        if(bin_term<1.0)
        {
            auto err = term * ( ( 1.0 - pow( mult_term, (double) (totalCount-i+1) ) ) /
                                  (1.0-mult_term) - 1.0 );

            if( err < tolerance * std::fabs(-std::log10(bin_tail)-logNT) * bin_tail ) break;
        }
    }
    return -log10(bin_tail) - logNT;
}

void RectImprover::computeNfaTable(double pInit, double logNt) {
    double p = pInit;
    int index = 0;
    for (int k = 0; k <= numImprovements; k++) {
        for (int totalCount = 0; totalCount < maxPopulation; totalCount++) {
            for (int goodCandidates = 0; goodCandidates <= totalCount; goodCandidates++) {
                // TODO: Check if once the value becomes false/true, it will always be false/true
                auto good_nfa = nfa(goodCandidates, totalCount, p, logNt);
                printf("Computing NFA table: %d/%d: %f\n", goodCandidates,totalCount,good_nfa);
                //Checl when the value becomes false, store the change position and stop
//                nfaTable[index] = nfa(i, j, p, logNt, 1.0);

            }
            index++;
        }
        p/=2;
    }
}

constexpr double RectImprover::logGammaLanczos(double x)
{
    double a, b = 0.0;
    a = (x + 0.5) * std::log(x + 5.5) - (x + 5.5);

    for (int n = 0; n < 7; ++n)
    {

        a -= std::log( x + static_cast<double>(n) );
        b += kLanczosCoefficients[n] * std::pow( x, static_cast<double>(n) );
    }
    return a + std::log(b);
}

constexpr double RectImprover::logGammaWindschitl(double x)
{
    return 0.918938533204673 + (x-0.5)*std::log(x) - x
           + 0.5*x*std::log( x*std::sinh(1/x) + 1/(810.0*std::pow(x,6.0)) );
}

constexpr double RectImprover::logGamma(double x) {
    if(x>15.0)
        return logGammaWindschitl(x);
    else
        return logGammaLanczos(x);
}

constexpr bool RectImprover::goodNfa(double nfa, double eps) {
    return nfa < eps;
}

