#include "a.h"
#include <iostream>

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

// specialization
template<>
void foo<float>()
{
  std::cout << "specialized foo" << std::endl;
}

// explicit instantiation
template class A<int>;

// explicit instantiation
template void foo<int>();
