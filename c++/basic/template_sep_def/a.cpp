#include "a.h"

//--------------------------------------------------------------------
template<typename T>
void A<T>::foo()
{
}

//--------------------------------------------------------------------
template<typename T>
void foo()
{
}

// notify compiler to compile these template with specific type.
template class A<int>;

void define_template_function(){
  foo<int>();
}

