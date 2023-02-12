//
// Created by Ibai Gorordo on 2023/02/12.
//

#include "utils.h"
#include <cmath>

// Ref: https://stackoverflow.com/questions/46210708/atan2-approximation-with-11bits-in-mantissa-on-x86with-sse2-and-armwith-vfpv4
float fast_atan2f(float y, float x) {
  float a, r, s, t, c, q, ax, ay, mx, mn;
  ax = std::fabs(x);
  ay = std::fabs(y);
  mx = std::fmax(ay, ax);
  mn = std::fmin(ay, ax);
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