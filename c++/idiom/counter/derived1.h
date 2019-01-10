#ifndef COUNTER_DERIVED0_H
#define COUNTER_DERIVED0_H

#include "counter.h"

class derived1 : private counter<derived1>
{
public:
  using counter::num_objects;
private:
  using counter::max_objects;
};


#endif /* COUNTER_DERIVED0_H */
