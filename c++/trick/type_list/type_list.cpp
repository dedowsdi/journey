#include <iostream>

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

// type_list
class null_type{};

template<typename H, typename T>
class type_list{};

#define type_list1(T1                                 ) type_list< T1, null_type>
#define type_list2(T1, T2                             ) type_list< T1, type_list1(T2) >
#define type_list3(T1, T2, T3                         ) type_list< T1, type_list2(T2,T3) >
#define type_list4(T1, T2, T3, T4                     ) type_list< T1, type_list3(T2,T3,T4) >
#define type_list5(T1, T2, T3, T4, T5                 ) type_list< T1, type_list4(T2,T3,T4,T5) >
#define type_list6(T1, T2, T3, T4, T5, T6             ) type_list< T1, type_list5(T2,T3,T4,T5,T6) >
#define type_list7(T1, T2, T3, T4, T5, T6, T7         ) type_list< T1, type_list6(T2,T3,T4,T5,T6,T7) >
#define type_list8(T1, T2, T3, T4, T5, T6, T7, T8     ) type_list< T1, type_list7(T2,T3,T4,T5,T6,T7,T8) >
#define type_list9(T1, T2, T3, T4, T5, T6, T7, T8, T9 ) type_list< T1, type_list8(T2,T3,T4,T5,T6,T7,T8,T9) >

// tl_length
template<typename TList> struct tl_length;

template<> struct tl_length<null_type>{
  enum {value = 0};
};

template<typename H, typename T> struct tl_length<type_list<H,T>>{
  enum {value = 1 + tl_length<T>::value};
};

// index
template<typename TList, unsigned index> struct tl_at;

template<typename H, typename T> struct tl_at< type_list<H,T>, 0>{
  typedef H result;
};

template<typename H, typename T, unsigned index> struct tl_at< type_list<H,T>, index>{
  typedef typename tl_at<T, index-1>::result result;
};

// indexof
template<typename TList, typename N> struct tl_indexof;

template<typename N> struct tl_indexof<null_type, N>{
  enum{value = -1}; // not found
};

template<typename H, typename T> struct tl_indexof<type_list<H,T>, H>{
  enum{value = 0}; // found at head
};

template<typename H, typename T, typename N> struct tl_indexof< type_list<H,T>, N>{
private:
  enum{temp = tl_indexof<T, N>::value}; // recurse, store value at temp.
public:
  enum{value = temp == -1 ? -1 : temp + 1};
};

// tl_append
template <typename TList, typename N> struct tl_append;

template <> struct tl_append<null_type, null_type>{
  typedef null_type result;
};

template <typename N> struct tl_append<null_type, N>{
  typedef type_list<N, null_type> result;
};

template <typename H, typename T> struct tl_append< type_list<H, T>, null_type>{
  typedef type_list<H, T> result;
};

template <typename H, typename T, typename N> struct tl_append< type_list<H, T>, N>{
  typedef type_list<H,  typename tl_append<T, N>::result > result; // recurse
};

// tl_erase
template <typename TList, typename U> struct tl_erase;

template <typename U> struct tl_erase< null_type, U>{
  typedef null_type result;
};

template <typename H, typename T> struct tl_erase< type_list<H,T>, H >{
  typedef T result;
};

template <typename H, typename T, typename U> struct tl_erase< type_list<H,T>, U >{
  typedef type_list<H, typename tl_erase<T,U>::result> result;
};

// erase_all
template <typename TList, typename U> struct erase_all;

template <typename U> struct erase_all< null_type, U>{
  typedef null_type result;
};

template <typename H, typename T> struct erase_all< type_list<H,T>, H >{
  // Go all the way down the list removing the type
  typedef typename erase_all<T, H>::result result;
};

template <typename H, typename T, typename U> struct erase_all< type_list<H,T>, U >{
  typedef type_list<H, typename erase_all<T,U>::result> result;
};

// tl_unique
template <typename TList> struct tl_unique;

template <> struct tl_unique<null_type>{
  typedef null_type result;
};

template <typename H, typename T> struct tl_unique< type_list<H,T> >{
  typedef typename tl_unique<T>::result temp; // recurse
  // remove duplicated head from tail
  typedef type_list<H, typename erase_all<temp, H>::result > result;
};

// tl_replace
template <typename TList, typename U, typename V> struct tl_replace;

template <typename U, typename V> struct tl_replace< null_type, U, V>{
  typedef null_type result;
};

template <typename H, typename T, typename V> struct tl_replace< type_list<H,T>, H, V >{
  typedef type_list<V,T> result;
};

template <typename H, typename T, typename U, typename V> struct tl_replace< type_list<H,T>, U, V >{
  typedef type_list<H, typename tl_replace<T,U, V>::result> result;
};

// tl_replace_all
template <typename TList, typename U, typename V> struct tl_replace_all;

template <typename U, typename V> struct tl_replace_all< null_type, U, V>{
  typedef null_type result;
};

template <typename H, typename T, typename V> struct tl_replace_all< type_list<H,T>, H, V >{
  typedef type_list<V, typename tl_replace_all<T,H, V>::result > result;
};

template <typename H, typename T, typename U, typename V> struct tl_replace_all< type_list<H,T>, U, V >{
  typedef type_list<H, typename tl_replace_all<T,U, V>::result> result;
};

// get most derived of U in TList, or U if not found
template<typename TList, typename U> struct tl_most_derived;

template<typename U> struct tl_most_derived<null_type, U>{
  typedef U result;
};

template<typename H, typename T, typename U> struct tl_most_derived< type_list<H,T>, U>{
private:
  typedef typename tl_most_derived< T,U >::result candidate;
public:
  typedef typename type_select< SUPERSUBCLASS(H, candidate), candidate, H>::result result;
};

// sort
template<typename TList> struct tl_shift_derived_to_front;

template<> struct tl_shift_derived_to_front<null_type>{
  typedef null_type result;
};

template<typename H, typename T> struct tl_shift_derived_to_front<type_list<H,T> >{
private:
  typedef typename tl_most_derived<T, H>::result most_derived_of_head_in_tail;
  // swap most derived and head
  typedef typename tl_replace<T, most_derived_of_head_in_tail, H>::result tail;
public:
  typedef type_list<most_derived_of_head_in_tail, tail> result;
};

typedef type_list4(char, short, int, long) signed_integrals;
typedef type_list2(float, double) floats;
typedef type_list3(unsigned char, unsigned short, unsigned int) unsigned_integrals3;
typedef tl_append<unsigned_integrals3, unsigned long>::result unsigned_ingegral;

typedef type_list4(int, long, long, long) i1l3;

struct A{
  A(){
    std::cout << "A()" << std::endl;
  }
  void foo(){
    std::cout << "A::foo()" << std::endl;
  }
};

struct B : public A{
  B(){
    std::cout << "B()" << std::endl;
  }
  void foo(){
    std::cout << "B::foo()" << std::endl;
  }
};

int main(int argc, char *argv[])
{
  std::cout << tl_length<signed_integrals>::value << std::endl;
  std::cout << tl_length<floats>::value << std::endl;
  std::cout << tl_length<unsigned_ingegral>::value << std::endl;
  std::cout << tl_length<tl_erase<unsigned_ingegral, unsigned short>::result >::value << std::endl;
  std::cout << tl_length<erase_all<i1l3, long>::result >::value << std::endl;
  std::cout << tl_length<tl_unique<i1l3>::result >::value << std::endl;
  std::cout << tl_length<tl_unique<tl_replace_all<i1l3, long, int>::result >::result>::value << std::endl;

  typedef type_list2(B,A) ba_list;
  typedef tl_most_derived<ba_list, A>::result C;
  C c;
  typedef tl_at<tl_shift_derived_to_front<ba_list>::result,0>::result D;
  D d;

  return 0;
}
