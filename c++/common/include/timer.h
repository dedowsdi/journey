#ifndef TIMER_H
#define TIMER_H
#include <chrono>

namespace zxd {
class Timer {
public:
  Timer() : mStart(std::chrono::steady_clock::now()) {}

  void reset() { mStart = std::chrono::steady_clock::now(); }

  std::chrono::steady_clock::duration time() {
    return std::chrono::steady_clock::now() - mStart;
  }

  double seconds()
  {
    return std::chrono::duration<double, std::ratio<1, 1>>(time()).count();
  }

  double miliseconds()
  {
    return std::chrono::duration<double, std::ratio<1, 1000>>(time()).count();
  }

protected:
  std::chrono::steady_clock::time_point mStart;
};
}

#endif /* TIMER_H */
