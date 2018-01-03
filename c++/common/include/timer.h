#ifndef TIMER_H
#define TIMER_H
#include <chrono>

namespace zxd {
class Timer {
public:
  Timer() : mStart(std::chrono::steady_clock::now()) {}

  inline void reset() { mStart = std::chrono::steady_clock::now(); }

  inline double time() {
    std::chrono::duration<double> elapsedTime =
      std::chrono::steady_clock::now() - mStart;
    return elapsedTime.count();
  }

protected:
  std::chrono::steady_clock::time_point mStart;
};
}

#endif /* TIMER_H */
