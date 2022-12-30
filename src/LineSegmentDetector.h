//
// Created by ibaig on 12/30/2022.
//

#ifndef TEST_LINESEGMENTDETECTOR_H
#define TEST_LINESEGMENTDETECTOR_H

#include "vector"
#include <numbers>
#include "Segment.h"
#include <cmath>

class LineSegmentDetector {

    double scale = 0.8;
    double sigma_scale = 0.6;
    double quant = 2.0;
    double ang_th = 22.5;
    double log_eps = 0.0;
    double density_th = 0.7;
    int n_bins = 1024;
    bool grad_nfa = true;
    double prec = std::numbers::pi * ang_th / 180.0;
    double p = ang_th / 180.0;
    double rho = quant / sin(prec); /* gradient magnitude threshold */

    public:
        std::vector<Segment> detect(double *gray_img,
                                    int width,
                                    int height);


};


#endif //TEST_LINESEGMENTDETECTOR_H
