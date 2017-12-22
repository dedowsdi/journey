#include <iostream>
#include <string>

const std::string indent("  ");

void lvalue() {
  std::cout << "lvalue:" << std::endl;
  std::cout << indent << "have identify and can not be moved are lvalue"
            << std::endl;
  std::cout << indent << "address of lvalue may be taken by &" << std::endl;
  std::cout << indent << "lvalue may be used to initialize an lvalue reference"
            << std::endl;
}

void xvalue() {
  std::cout << "xvalue: expiring value" << std::endl;
  std::cout << indent << "have identify and can be moved are xvalue"
            << std::endl;
  std::cout << indent
            << "If a is rvalue object or array, a.m, a.*mp, a[n] are all rvalue"
            << std::endl;
}

void prvalue() {
  std::cout << "pvalue: pure rvalue" << std::endl;
  std::cout << indent << "do not have identify and can be moved are prvalue"
            << std::endl;
}

void glvalue() {
  std::cout << "glvalue: generalized lvalue" << std::endl;
  std::cout << indent << "expression that have identify are called glvalue. "
                         "It's either lvalue or xvalue"
            << std::endl;
  std::cout << indent << "A glvalue may be polymorphic" << std::endl;
  std::cout << indent << "A glvalue can have incomplete type" << std::endl;
}

void rvalue() {
  std::cout << "rvalue:" << std::endl;
  std::cout << indent << "expression that can be moved from are rvalue. It's "
                         "either prvalue or xvalue"
            << std::endl;
  std::cout << indent << "address of rvalue can not be taken by &" << std::endl;
  std::cout
    << indent
    << "an rvalue may be used to initialize an lvalue or rvalue reference, in "
       "which case the life time of the object identified by the "
       "rvalue is extended until the scope of the reference ends "
    << std::endl;
}

void expression() {
  std::cout << "expression: glvalue or rvalue, or both(xvalue)" << std::endl;
}

int main(int argc, char *argv[]) {
  expression();
  glvalue();
  rvalue();
  lvalue();
  xvalue();
  prvalue();

  return 0;
}
