#include <iostream>

struct Data
{
  bool b;
  int i;
  float f;
};

class Resource
{
protected:
  Data* mData;
public:

  // custom swap.
  // another way(suggest by effective c++) to do this is define swap as menber,
  // then add a nonmenber swap in the same namespace, a specialization swap for this
  // type in std namespace(in case someone use std::swap for this type), if
  // this class is a template, use overload instead of specialization.
  friend void swap(Resource& r0, Resource& r1) noexcept  // most of time, no throw
  {
    std::swap(r0.mData, r1.mData);
  }

  Resource()
  {
    mData = new Data();
  }

  ~Resource()
  {
    if(mData)
      delete mData;
  }

  // deep copy
  Resource(const Resource& r)
  {
    mData = new Data(*r.mData);
  }

  // in order to discard original data, we use pass by value, the parameter
  // object will help us to deal with original data, if you pass parameter by r
  // value, no copy exists during parameter init.
  Resource& operator=(Resource r)
  {
    using std::swap;
    swap(*this, r);
    return *this;
  }

  // move operator for c++11
  Resource(Resource&& r)
  {
    using std::swap;
    swap(*this, r);
  }

};

int main(int argc, char *argv[])
{
  return 0;
}
