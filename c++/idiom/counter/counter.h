#ifndef COUNTER_COUNTER_CPP
#define COUNTER_COUNTER_CPP
#include <stdexcept>

template <typename T>
class counter
{
protected:

  class too_many_objects{};

  counter(){
    if(num_objects >= max_objects)
      throw too_many_objects{};

    ++num_objects;
  }
   // non virtual, this base class will be derived privately, visible only to
   // derived
  ~counter(){ --num_objects;}
  counter(const counter& c){ ++num_objects;}
  counter& operator=(const counter& c) = default;

  static int object_count(){return num_objects;}

  static int num_objects; // defined in counter.cpp
  static const int max_objects; // need to be defined by derived class
};

#endif /* COUNTER_COUNTER_CPP */
