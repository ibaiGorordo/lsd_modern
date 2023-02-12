//
// Created by ibaig on 1/1/2023.
//

#include "cmath"
#include "algorithm"
#include "GradientCalculator.h"

// Ref: https://stackoverflow.com/questions/46210708/atan2-approximation-with-11bits-in-mantissa-on-x86with-sse2-and-armwith-vfpv4
float fast_atan2f(float y, float x) {
  float a, r, s, t, c, q, ax, ay, mx, mn;
  ax = fabsf(x);
  ay = fabsf(y);
  mx = fmaxf(ay, ax);
  mn = fminf(ay, ax);
  a  = mn / mx;
  /* Minimax polynomial approximation to atan(a) on [0,1] */
  s = a * a;
  c = s * a;
  q = s * s;
  r = 0.024840285f * q + 0.18681418f;
  t = -0.094097948f * q - 0.33213072f;
  r = r * s + t;
  r = r * c + a;
  /* Map to full circle */
  if (ay > ax) r = 1.57079637f - r;
  if (x < 0) r = 3.14159274f - r;
  if (y < 0) r = -r;
  return r;
}

void GradientCalculator::calculateGradients(const unsigned char *image,
                                            int width, int height,
                                            double *angles,
                                            double *magnitudes,
                                            unsigned char *bad_pixels) const {

  std::fill(angles, angles + width * height, -1024.0);

#pragma omp parallel for collapse(2)
    // Compute the gradient on the image
    for (int y = 0; y < height - 1; y++) {
        auto row_offset = y * width;
        for (int x = 0; x < width - 1; x++) {
            const auto index = row_offset + x;
            const auto DA = image[index + width + 1] - image[index];
            const auto BC = image[index + 1] - image[index + width];
            const auto dx = DA + BC;
            const auto dy = DA - BC;
            const auto norm = dx * dx + dy * dy;
            const auto is_bad = norm <= threshold_squared;
            if (is_bad) {
                bad_pixels[index] = is_bad;
                continue;
            }
            bad_pixels[index] = is_bad;
            magnitudes[index] = std::sqrt(norm)/2.0;
            angles[index] = fast_atan2f((float) dx, (float) -dy);

        }
    }
}
