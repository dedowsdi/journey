// use traits to check pointer or reference
#include <iostream>
#include <vector>

class NullType{};

template<typename T>
class type_traites
{

private:
  
  // pointer
  template<typename U>
  struct pointer_traits{
    enum {result = false};
    typedef NullType pointee_type;
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
    typedef U referenced_type; // careful, not NullType
  };

  template<typename U>
  struct reference_traits<U&>{
    enum {result = true};
    typedef U referenced_type;
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
    is_member_pointer = pointer_to_member_traits<T>::result
  };
  typedef typename pointer_traits<T>::pointee_type pointee_type;
  typedef typename reference_traits<T>::referenced_type referenced_type;

};

class A{
public:
  int i;
};

int main(int argc, char *argv[])
{

  std::cout << "is int pointer ? " << type_traites<int>::is_pointer << std::endl;
  std::cout << "is int* pointer ? " << type_traites<int*>::is_pointer << std::endl;
  std::cout << "is std::vector<int>::iterator pointer ? " 
            << type_traites< std::vector<int>::iterator >::is_pointer << std::endl;
  std::cout << "is std::vector<A>::iterator pointer ? " 
            << type_traites< std::vector<A>::iterator >::is_pointer << std::endl;

  std::cout << "is int reference ? " << type_traites<int>::is_reference << std::endl;
  std::cout << "is int& reference ? " << type_traites<int&>::is_reference << std::endl;
  
  return 0;
}
