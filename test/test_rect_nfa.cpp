//
// Created by Ibai Gorordo on 2023/03/08.
//

#include "RectImprover.h"
#include <cmath>
#include <numbers>


int main() {
    auto log_nt = 5 * (std::log10(double(1280)) + std::log10(double(1280))) / 2;
    auto rectImprover = RectImprover(22.5 * std::numbers::pi / 180.0, log_nt);

    return 0;
}