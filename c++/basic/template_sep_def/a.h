template<typename T>
class A
{
  public:
    void foo();
};

template<typename T>
void foo();


// The code compiles fine without following declaration, but declare them make
// things clear ?

extern template class A<int>;

template <>
void foo<float>();

extern template void foo<int>();
