//
// Created by ibaig on 12/30/2022.
//

#ifndef TEST_LINESEGMENTDETECTOR_H
#define TEST_LINESEGMENTDETECTOR_H

#include "vector"
#include <numbers>
#include "Segment.h"
#include "GaussianDownsampler.h"
#include <cmath>

class LineSegmentDetector {

    public:
        explicit LineSegmentDetector(double scale=0.8,
                            double sigma_scal=0.6,
                            double quant=2.0,
                            double ang_th=22.5,
                            double log_eps=0,
                            double density_th=0.7,
                            int n_bins=1024);
        std::vector<Segment> detect(double *gray_img,
                                    int width,
                                    int height);

    private:
        double scale;
        double sigma_scale;
        double quant;
        double ang_th;
        double log_eps;
        double density_th;
        int n_bins;
        bool grad_nfa = true;
        bool refine = true;
        double prec;
        double p;
        double rho;

        double *smoothed_img;
        GaussianDownsampler gaussian_downsampler;


};


#endif //TEST_LINESEGMENTDETECTOR_H
