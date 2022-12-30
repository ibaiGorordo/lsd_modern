//
// Created by ibaig on 12/30/2022.
//

#include "LineSegmentDetector.h"

LineSegmentDetector::LineSegmentDetector(double scale, double sigma_scale, double quant, double ang_th, double log_eps,
                                         double density_th, int n_bins) {
    this->scale = scale;
    this->sigma_scale = sigma_scale;
    this->quant = quant;
    this->ang_th = ang_th;
    this->log_eps = log_eps;
    this->density_th = density_th;
    this->n_bins = n_bins;

    prec = std::numbers::pi * ang_th / 180.0;
    p = ang_th / 180.0;
    rho = quant / std::sin(prec);
}

std::vector<Segment> LineSegmentDetector::detect(double *gray_img, int width, int height) {
    std::vector<Segment> segments;
    return segments;
}