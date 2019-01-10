#ifndef COUNTER_DERIVED0_H
#define COUNTER_DERIVED0_H

#include "counter.h"

class derived0 : private counter<derived0>
{
public:
  using counter::object_count;
  using counter::too_many_objects;
};


#endif /* COUNTER_DERIVED0_H */
