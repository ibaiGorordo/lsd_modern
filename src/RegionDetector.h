//
// Created by ibaig on 1/3/2023.
//

#ifndef TEST_REGIONDETECTOR_H
#define TEST_REGIONDETECTOR_H

struct NormPoint {
    int x;
    int y;
    uint16_t quant_norm{};

    explicit NormPoint(int x, int y, unsigned int quant_norm) : x(x), y(y), quant_norm(quant_norm) {}

    bool operator > (const NormPoint &other) const
    {
        return quant_norm > other.quant_norm;
    }
};


class RegionDetector {

public:
    RegionDetector() = default;
    explicit RegionDetector(double threshold=22.5);

    void detect(const double *grad_x,
                const double *grad_y,
                const double *magnitudes,
                const unsigned char *bad_pixels,
                int width, int height);

private:
    double min_reg_size{};
    int img_width = 0;
    int img_height = 0;

    double ang_th{};
    double p{};

    static constexpr uint16_t num_bins = 1024*4;
    static constexpr uint16_t max_grad = 256;
    static constexpr double quant_coeff = (double) num_bins / max_grad;
    std::vector<NormPoint> sorted_pixels;


private:
    void search_regions(const double *grad_x,
                   const double *grad_y,
                   const double *magnitudes,
                   const unsigned char *bad_pixels);
    void get_sorted_pixels(const double *magnitudes, const unsigned char *bad_pixels);
    void check_new_img_size(int width, int height);

};


#endif //TEST_REGIONDETECTOR_H
