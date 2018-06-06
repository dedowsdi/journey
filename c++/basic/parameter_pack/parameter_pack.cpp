#include <iostream>
#include <tuple>

template<typename ...Types>
std::tuple<Types...> make_tuple(){ // Types... will be expanded to deduced types
  return std::tuple<Types...>();
}

// you can use parameter packer to do something via recursion

// base function. !you must define this, as compiler will generate all possible
// branches. If you don't define this, and use following code to control border
// problem:
//    if(count >=1)
//      print(args...)
// it will fail!. because when count=1, compiler will compile print(value), and
// then, count=0, compiler will compile print(), which doesn't exist!!!!
template<typename T>
void print(T value){
  std::cout << value << std::endl;
}

// recursive print
template<typename T, typename... Types>
void print(T value, Types... args){
  int count = sizeof...(args); // sizeof... return number of types in packed parameter
  //std::cout << count << std::endl;
  std::cout << value << std::endl;
  print(args...); // args... is called expanded to arg0, arg1 .....
}

int main(int argc, char *argv[])
{
   auto tp0 = make_tuple<int, float, double>();
   std::get<0>(tp0) = 0;
   std::get<1>(tp0) = 1.0;
   std::get<2>(tp0) = 2.0;
   
   print(1,2,3,4,5);
  
  return 0;
}
