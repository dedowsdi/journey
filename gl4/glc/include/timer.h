#ifndef GL_GLC_TIMER_H
#define GL_GLC_TIMER_H
#include <chrono>

class timer
{
protected:
  std::chrono::steady_clock::time_point m_start;

public:
  timer(std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now());
  void reset();
  std::chrono::steady_clock::duration time();
  float time_seconds();
  float time_miliseconds();

};


#endif /* GL_GLC_TIMER_H */
