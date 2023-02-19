//
// Created by Ibai Gorordo on 2023/02/17.
//

#ifndef TEST_REGIONPOINT_H
#define TEST_REGIONPOINT_H

struct RegionPoint {
  int x;
  int y;
  double angle;
  double norm;

  RegionPoint() = default;
  explicit RegionPoint(int x, int y, double angle, double norm) : x(x), y(y), angle(angle), norm(norm) {}
};

#endif // TEST_REGIONPOINT_H
