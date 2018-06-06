#ifndef GEN_SCATTER_HIERACHY_H
#define GEN_SCATTER_HIERACHY_H
#include "type_list.h"

namespace mp{

// generate class that inherits every Unit<T>, T belongs to TList.
template<typename TList, template<typename >class Unit>
class gen_scatter_hierachy;

// specialization for null_type
template< template<typename >class Unit>
class gen_scatter_hierachy<null_type, Unit>{
};

// atomic type, inherits from Unit<T>
template<typename T, template<typename>class Unit>
class gen_scatter_hierachy: public Unit<T>{
public:
    typedef Unit<T> left_brace;
};

// specialization for type_list, recursively inherits from left_brace and
// right_brace
template<typename H, typename T, template<typename>class Unit>
class gen_scatter_hierachy< type_list<H,T>, Unit>:
  public gen_scatter_hierachy<H, Unit>,
  public gen_scatter_hierachy<T, Unit>{
  public:
    typedef type_list<H,T> t_list;
    typedef gen_scatter_hierachy<H, Unit> left_brace;
    typedef gen_scatter_hierachy<T, Unit> right_brace;
};

// private
namespace pri{

// overload for 0 in dex
template<typename T, typename U>
U& field_helper(T& obj, type2type<U>, int2type<0>){
  typename T::left_brace& obj2 = obj;
  return obj2;
}

template<typename T, typename U, int i>
U& field_helper(T& obj, type2type<U> tt, int2type<i>){
  typename T::right_brace& obj2 = obj;
  return field_helper(obj2, tt, int2type<i-1>());
}

/*
 * by default, you need to access certain parent field this:
 *  std::static_cast<Unit<T>& >(obj).fieldName, it's augly. with the help of
 *  field_traits and field, you can do it like this:
 *    field<T>(info).fieldName
 */
template<typename T>
struct field_traits{
  template<typename U>
  struct rebind;
  
  template<unsigned i>
  struct at;
};


template <typename TList, template<typename>class Unit>
struct field_traits< gen_scatter_hierachy<TList, Unit>>{
  template<typename U>
  struct rebind{
    typedef Unit<U> result;
  };

  template<unsigned i>
  struct at{
    // get type of Unit<type at index i>
    typedef Unit<typename tl_at<TList, i>::result> result;
  };
};

}

// access Unit<T> by T
template<typename T, typename U>
typename pri::field_traits<U>::template rebind<T>::result& field(U& obj){
  return obj;
}

// access Unit<T> by index of T in type_list
template<unsigned i, typename U>
typename pri::field_traits<U>::template at<i>::result& field(U& obj){
  typedef typename pri::field_traits<U>::template at<i>::result result;
  return pri::field_helper(obj, type2type<result>(), int2type<i>());
}

}

#endif /* GEN_SCATTER_HIERACHY_H */
