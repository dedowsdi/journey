/*
 *  curiously recurring template pattern is als knowns as F-bound polymorphism.
 *  A class X derives from a base class template instantiation using X itself as
 *  template argument, you can then use derived class in base class without
 *  declare it first.
 */

#include <iostream>
#include <string>

template <typename T>
struct counter {
  static int objects_created;
  static int objects_alive;

  counter() {
    ++objects_created;
    ++objects_alive;

    // you can call derived function even in ctor! you can never achive this
    // with dynamic polymorphism
    T* t = static_cast<T*>(this);
    std::cout << "create " << t->getType() << ". alive:" << objects_alive
              << " created:" << objects_created << std::endl;
  }

  counter(const counter &) {
    ++objects_created;
    ++objects_alive;
  }

protected:
  // objects should never be removed through pointers of this type, it's not
  // virtual.
  ~counter()  
  {
    --objects_alive;
    T* t = static_cast<T*>(this);
    std::cout << "destroy " << t->getType() << ". alive:" << objects_alive
              << " created:" << objects_created << std::endl;
  }
};

template <typename T>
int counter<T>::objects_created(0);
template <typename T>
int counter<T>::objects_alive(0);

class X : public counter<X> {
  public :
    static std::string getType(){return "x";}
};

class Y : public counter<Y> {
  public :
    static std::string getType(){return "y";}
};

int main(int argc, char *argv[]) { 
  {
    X x;
    Y y;
  }
  return 0; 
}
