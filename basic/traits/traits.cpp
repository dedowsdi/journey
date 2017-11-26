/*
 * Traits classes make information about types available during compilation.
 * They're implemented using templates and template specializations.
 *
 * In conjunction with overloading, traits classes make it possible to perform
 * compile-time if...else tests on types.
 *
 * Traits represent traits, they are useually not specific class, you can not
 * overload them directly, you need to create some empty struct to represent the
 * trait, then use typedef to give this trait to it's host. In order to use
 * trait, you also need a trait struct, which just parrot the typdef in the
 * hosts, so the overload will work based on the parroted type
 */
#include <iostream>
#include <string>

//empty struct represent specific trait
struct Move_Run {};
struct Move_Fly {};

class bird {
public:
  std::string getType() const{ return "bird"; }
  typedef Move_Fly Move_Tag; //typedef give trait to it's host
};

class dog {
public:
  std::string getType() const { return "run"; }
  typedef Move_Run Move_Tag; //typedef give trait to it's host
};

// trait class simply parrot the type in trait host
template <typename T>
struct Move_Traits {
  typedef typename T::Move_Tag Move_Tag;
};

template <typename T>
void move(const T& t) {
  doMove(t, typename Move_Traits<T>::Move_Tag());
}

//now we can use overload on this trait
template <typename T>
void doMove(const T& t, Move_Run) {
  std::cout << t.getType() << " can run" << std::endl;
}

template <typename T>
void doMove(const T& t, Move_Fly) {
  std::cout << t.getType() << " can fly" << std::endl;
}

int main(int argc, char* argv[]) {
  bird b;
  dog d;
  move(b);
  move(d);

  return 0;
}
