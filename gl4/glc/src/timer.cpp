#include <timer.h>

//--------------------------------------------------------------------
timer::timer(std::chrono::steady_clock::time_point start):
  m_start(start)
{
}

//--------------------------------------------------------------------
void timer::reset()
{
  m_start = std::chrono::steady_clock::now();
}

//--------------------------------------------------------------------
std::chrono::steady_clock::duration timer::time()
{
  return std::chrono::steady_clock::now() - m_start;
}

//--------------------------------------------------------------------
float timer::time_seconds()
{
  std::chrono::duration<double, std::ratio<1, 1>> d(time());
  return d.count();
}

//--------------------------------------------------------------------
float timer::time_miliseconds()
{
  std::chrono::duration<double, std::ratio<1, 1000>> d(time());
  return d.count();
}
