/*
 *  if you really want to define your template class or function in source file
 *  without include it in head file, you can do something like this in the end
 *  of source
 *  file:
 *    template class ClassName<SpecificType0>;
 *    template class ClassName<SpecificType1>;
 *    ...
 *    template class ClassName<SpecificTypen>;
 *    void define_template_function(){
 *      void func<SpecificType0();
 *      void func<SpecificType1();
 *      ...
 *      void func<SpecificType2();
 *    }
 *
 *  it will cause compiler to compile these particular types, so the associated
 *  class member functions and global template functions will be available at
 *  link time.
 */

#include <iostream>
#include "a.h"

int main(int argc, char *argv[]) {
  A<int> a0;
  a0.foo();

  foo<int>();

  return 0;
}
