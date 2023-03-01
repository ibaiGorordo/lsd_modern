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
  if (ay > ax) r = half_pi - r;
  if (x < 0) r = pi - r;
  if (y < 0) r = -r;
  return r;
}

double fast_atan2d(double y, double x) {

    double a, r, s, t, c, q, ax, ay, mx, mn;
    ax = std::fabs(x);
    ay = std::fabs(y);
    mx = std::fmax(ay, ax);
    mn = std::fmin(ay, ax);
    a  = mn / mx;
    /* Minimax polynomial approximation to atan(a) on [0,1] */
    s = a * a;
    c = s * a;
    q = s * s;
    r = 0.024840285 * q + 0.18681418;
    t = -0.094097948 * q - 0.33213072;
    r = r * s + t;
    r = r * c + a;
    /* Map to full circle */
    if (ay > ax) r = half_pi - r;
    if (x < 0) r = pi - r;
    if (y < 0) r = -r;
    return r;
}

double wrap_angle_signed(double angle, double limit) {
    double new_angle = angle;
    if ( new_angle < -limit ) new_angle += two_pi;
    else if ( new_angle > limit ) new_angle -= two_pi;
    return new_angle;
}


double wrap_angle(double angle, double limit) {
    double new_angle = wrap_angle_signed(angle, limit);
    if ( new_angle < 0 ) new_angle = -new_angle;
    return new_angle;
}


double angle_diff_signed(const double& angle1, const double&angle2)
{
    const double diff = angle1 - angle2;
    return wrap_angle_signed(diff);
}

double angle_diff(const double& angle1, const double& angle2)
{
    const double diff = angle1 - angle2;
    return wrap_angle(diff);
}

double point_dist_squared(const double& x1, const double& y1, const double& x2, const double& y2) {
    const double dx = x2 - x1;
    const double dy = y2 - y1;
    return dx*dx + dy*dy;
}

double point_dist(const double& x1, const double& y1, const double& x2, const double& y2) {
    return std::sqrt(point_dist_squared(x1, y1, x2, y2));
}