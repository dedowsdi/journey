/*
 * duration : a span of time, defined by number of ticks and some time units.
 * clock : epoch and a tick rate.
 *   system_clock : wall clock time from the system-wide real time clock
 *   steady_clock : monotonic clock that will never be adjusted
 *   high_resolution_clock : the clock with shortest tick period available.
 * time point : a duration of time that has passed since the epoch of specified
 *              clock
 * time_t : arithmetic type capable of representing time
 *
 *
 */
#include <iostream>
#include <chrono>
#include <ctime>

void now() {
  std::cout << "******************************************" << std::endl;
  std::cout << "now:" << std::endl;
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t nowt = std::chrono::system_clock::to_time_t(now);
  std::cout << std::ctime(&nowt);
}

void timer() {
  std::cout << "******************************************" << std::endl;
  std::cout << "timer:" << std::endl;
  std::chrono::time_point<std::chrono::steady_clock> start, end;
  start = std::chrono::steady_clock::now();
  std::cout << "do some printing and timer it" << std::endl;
  end = std::chrono::steady_clock::now();
  // by default duration use second as time unit
  std::chrono::duration<double> elapsedTime = end - start;
  std::cout << "job done, it tooks :" << elapsedTime.count()
            << "seconds to do the printing" << std::endl;
}

void convert() {
  std::cout << "******************************************" << std::endl;
  std::cout << "convert:" << std::endl;
  std::chrono::seconds sd(5);
  std::chrono::milliseconds md(sd);
  std::chrono::microseconds ud(sd);
  std::cout << sd.count() << " seconds " << std::endl;
  std::cout << md.count() << " milliseconds " << std::endl;
  std::cout << ud.count() << " microseconds " << std::endl;

  std::chrono::duration<double, std::ratio<1> > dsd(0.5);
  std::chrono::duration<double, std::ratio<1, 1000>> dmd(dsd);
  std::chrono::duration<double, std::ratio<1, 1000000>> dud(dsd);

  std::cout << dsd.count() << " seconds " << std::endl;
  std::cout << dmd.count() << " milliseconds " << std::endl;
  std::cout << dud.count() << " microseconds " << std::endl;
}

int main(int argc, char *argv[]) {
  now();
  timer();
  convert();
  return 0;
}
