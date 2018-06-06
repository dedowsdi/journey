#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H
#include "type_util.h"

namespace mp{

template<typename T>
class type_traites
{

private:
  
  // pointer
  template<typename U>
  struct pointer_traits{
    enum {result = false};
    typedef null_type pointee_type;
  };

  template<typename U>
  struct pointer_traits<U*>{
    enum {result = true};
    typedef U pointee_type;
  };

  // reference
  template<typename U>
  struct reference_traits{
    enum {result = false};
    typedef U referenced_type; // careful, not null_type
  };

  template<typename U>
  struct reference_traits<U&>{
    enum {result = true};
    typedef U referenced_type;
  };

  // const stripper
  template<typename U>
  struct const_traits{
    enum {result = false};
    typedef U unconst_type;
  };

  template<typename U>
  struct const_traits<const U>{
    enum {result = true};
    typedef U unconst_type;
  };

  // pointer to member, horrible.
  template <class U> struct pointer_to_member_traits
  {
    enum { result = false };
  };

  template <class U, class V>
  struct pointer_to_member_traits<U V::*> // @Ques know how to read this.
  {
    enum { result = true };
  };

public:

  enum {
    is_pointer = pointer_traits<T>::result,
    is_reference = reference_traits<T>::result,
    is_member_pointer = pointer_to_member_traits<T>::result,
    is_const_qualified = const_traits<T>::result
  };
  typedef typename pointer_traits<T>::pointee_type pointee_type;
  typedef typename reference_traits<T>::referenced_type referenced_type;
  typedef typename const_traits<T>::unconst_type unconst_type;

};

}

#endif /* TYPE_TRAITS_H */
