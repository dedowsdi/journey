#ifndef TYPE_UTIL_H
#define TYPE_UTIL_H

namespace mp{

class null_type{};
struct empty_type{};

// select
template< bool b, typename T, typename U>
struct type_select{
  typedef T result;
};

template<typename T, typename U>
struct type_select<false,T,U>{
  typedef U result;
};

template <typename T, typename U>
class conversion {
  typedef char Small;
  class Big { char dummy[2]; };

  static Small test(U);
  static Big test(...);

  static T MakeT();

public:
  enum { 
    exists = sizeof(test(MakeT())) == sizeof(Small), 
    sameType = false 
  };
};

// specialize to same type
template <typename T>
class conversion<T, T> {
public:
  enum { exists = 1, sameType = true };
};

#define SUPERSUBCLASS(T, U)                  \
  (conversion<const U*, const T*>::exists && \
    !conversion<const T*, const void*>::sameType)

template <int v>
struct int2type {
  enum { value = v };
};

template <typename T>
struct type2type {
  typedef T original_type;
};

}

#endif /* TYPE_UTIL_H */
