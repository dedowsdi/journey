/*
 * Local classes are final. Outside users cannot derive from a class hidden in a
 * function. Without local classes, you'd have to add an unnamed namespace in a
 * separate translation unit.
 */
#include <iostream>

class Interface {
public:
  virtual ~Interface(){}
  virtual void fun() = 0;
};

template <class T, class P>
Interface* makeAdapter(const T& obj, const P& arg) {
  //you can define local class inside function, you can not access it directly
  //outside of this fuction.
  class Local : public Interface {
  public:
    Local(const T& obj, const P& arg) : obj_(obj), arg_(arg) {}
    virtual void fun() { obj_.funA(arg_); }

  private:
    T obj_;
    P arg_;
  };
  return new Local(obj, arg);
}

class A {
public:
  void funA(int i) {
    std::cout << "A is having fun for " << i << " hours " << std::endl;
  }
};

void haveFun(Interface* _if) { _if->fun(); }

int main(int argc, char* argv[]) {
  A a;
  Interface* _if = makeAdapter(a, 5);
  haveFun(_if);
  delete _if;

  return 0;
}
