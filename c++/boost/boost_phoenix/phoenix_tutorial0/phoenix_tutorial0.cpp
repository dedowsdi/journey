#include <iostream>
#include <boost/phoenix.hpp>
#include <string>

namespace bp = boost::phoenix;

template <typename F>
void print(F f)
{
  std::cout << f() << std::endl;
}

int main(int argc, char *argv[]) { 

  // bp::val returns a function
  // val(3) returns a nullary function (a functions takes no arguments)
  print(bp::val(3));
  print(bp::val("hello phonix"));

  auto i = 5;
  auto ir = bp::ref(i);
  ir() = 4;
  std::cout << i << std::endl;
   // phoenix override lots of operator, this expression will create a new
   // function which set i to 2
  auto ir2 = ir = 2;
  ir2();
  std::cout << i << std::endl;

  std::string s = "it's a string";
  print(bp::cref(s));

  return 0;
}
