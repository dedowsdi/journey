#include <iostream>
#include <memory>

class RaiiInt
{
public:
  RaiiInt(int* data):
    mData(data)
  {
  }

  ~RaiiInt()
  {
    if(mData)
    {
      std::cout << "clear data: " << *mData << std::endl;
      delete mData;
    }
  }

  // lots of raii provide explicit access to underlying resource
  int* get()
  {
    return mData;
  }
private:
  // if resource forbid copy, you should also forbid copy of raii object
  RaiiInt(const RaiiInt& rhs) = delete;
  RaiiInt* operator=(RaiiInt rhs) = delete;

  int* mData;
};

class Resource
{

};

struct ResourceDeleter
{
  // you can do whatever needed to release release
  void operator()(Resource* r)
  {
    delete r;
    std::cout << "release resource" << std::endl;
  }
};

// support reference count and custom unlock(delete)
class RaiiResource
{
public:
  RaiiResource(Resource* r) :
    mResource(r, ResourceDeleter())
  {
  }

  // no destructor, it's handled by shared_ptr
private:
  std::shared_ptr<Resource> mResource;
};


int main(int argc, char *argv[])
{
  {
    RaiiInt ri(new int(6));
    RaiiResource r0(new Resource);
    RaiiResource r1 = r0;
  }
  return 0;
}
