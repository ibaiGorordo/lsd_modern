//
// Created by Ibai Gorordo on 2023/02/12.
//

#ifndef TEST_UTILS_H
#define TEST_UTILS_H


constexpr double half_pi = 1.5707963267948966192313216916398;
constexpr double pi = 3.1415926535897932384626433832795;
constexpr double pi_and_half = 4.7123889803846898576939650749193;
constexpr double two_pi = 6.283185307179586476925286766559;

float fast_atan2f(float y, float x);
double fast_atan2d(double y, double x);

double wrap_angle_signed(double angle, double limit = pi);
double wrap_angle(double angle, double limit = pi);
double angle_diff_signed(const double& angle1, const double& angle2);
double angle_diff(const double& angle1, const double& angle2);
double point_dist(const double& x1, const double& y1, const double& x2, const double& y2);



#endif // TEST_UTILS_H
