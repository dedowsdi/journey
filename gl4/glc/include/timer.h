#ifndef GL_GLC_TIMER_H
#define GL_GLC_TIMER_H
#include <chrono>

class timer
{
public:

  using clock = std::chrono::steady_clock;
  using dseconds = std::chrono::duration<double, std::ratio<1,1>>;
  using dmilliseconds = std::chrono::duration<double, std::ratio<1,1000>>;
  using dmicroseconds = std::chrono::duration<double, std::ratio<1,1000000>>;

  timer();

  void reset() { _start = clock::now(); }

  double seconds()
  {
    return std::chrono::duration_cast<dseconds>(get()).count();
  }

  double milliseconds()
  {
    return std::chrono::duration_cast<dmilliseconds>(get()).count();
  }

  double microseconds()
  {
    return std::chrono::duration_cast<dmicroseconds>(get()).count();
  }

private:

  dmilliseconds get() { return clock::now() - _start; }
  clock::time_point _start;
};


#endif /* GL_GLC_TIMER_H */
