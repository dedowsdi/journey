/*
 *  curiously recurring template pattern is als knowns as F-bound polymorphism.
 *  A class X derives from a base class template instantiation using X itself as
 *  template argument, you can then use derived class in base class without
 *  declare it first.
 *
 *  https://www.fluentcpp.com/2017/05/16/what-the-crtp-brings-to-code/
 *  no virtual exists in crtp base. It's mainly used for two purpose:
 *    adding functionality
 *    create static interface
 */

#include <iostream>
#include <string>

// a crtp help class:
// can be used like this:
//      template<typename T>
//      class crtp : public crtp_base<T, crtp>
// 
template <typename T, template<class> class DerivedPhantom>
struct crtp_base
{
    T& underlying() { return static_cast<T&>(*this); }
    T const& underlying() const { return static_cast<T const&>(*this); }
private:
    // only derived child cal call it.
    crtp_base(){}
    friend DerivedPhantom<T>;
};


template <typename T>
struct counter {
  static int objects_created;
  static int objects_alive;

  // if you want, you can declare ctor and copy ctor private, and add friend
  // T, so you make sure only something like this doesn't happen:
  //  class A : public counter<B>
  //  {
  //      ...
  //  };
  //
  counter() {
    ++objects_created;
    ++objects_alive;

    // counter should only be inherited by T, so counter 100% can be converted
    // to T
    // you can call derived function even in ctor! you can never achieve this
    // with dynamic polymorphism.
    T* t = static_cast<T*>(this);
    std::cout << "create " << t->getType() << ". alive:" << objects_alive
              << " created:" << objects_created << std::endl;
  }

  counter(const counter &) {
    ++objects_created;
    ++objects_alive;
  }

  // functionality in ctrp, no virtual version of virtual method
  void function0()
  {
    T& t = underlying();
    float paramter = t.get_unique_something();
    // do something
  }

  void function1()
  {
    T& t = underlying();
    float paramter = t.get_unique_something();
    // do something
  }

  // static interface, polymorphying without virtual
  float get_something()
  {
    T& t = underlying();
    t.get_something();
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

private:
  T& underlying() { return static_cast<T&>(*this); }
  const T& underlying() const { return static_cast<const T&>(*this); }

};

template <typename T>
int counter<T>::objects_created(0);
template <typename T>
int counter<T>::objects_alive(0);

class X : public counter<X> {
public :
  static std::string getType(){return "x";}
  float get_unique_something(){return 0;}
  float get_something(){return 0;}
};

class Y : public counter<Y> {
public :
  static std::string getType(){return "y";}
  float get_unique_something(){return 1;}
  float get_something(){return 0;}
};

int main(int argc, char *argv[]) { 
  {
    X x;
    Y y;
    x.function0();
    float f = x.get_something();
  }

  return 0; 
}
