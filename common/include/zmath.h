#ifndef ZMATH_H
#define ZMATH_H

#include <cmath>
namespace zxd {

const double pi = 3.14159265358979323846264338327950288419716939937510;
const double e = 2.71828182845904523536028747135266249775724709369995;
class ZMath {
public:
  static double gaussian(double x, double mean, double deviation) {
    static double r = 1 / std::sqrt(2 * pi);
    double rd = 1 / deviation;
    return std::pow(e, -0.5 * std::pow((x - mean) * rd, 2)) * rd * r;
  }
};
}

#endif /* ZMATH_H */
