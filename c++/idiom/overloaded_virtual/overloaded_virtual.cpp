#include <iostream>

class Base
{
public:
    virtual ~Base() = default;

    // use the same name for overloaded public non-virtual
    void f( int v )
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        f_int( v );
    }

    void f( double v )
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        f_double( v );
    }

protected:
    // use different name for overloaded protected virtual
    virtual void f_int( int v ) {}

    virtual void f_double( double v ) {}
};

class Derived : public Base
{

protected:
    // when you call derived->f(int) or derived->f(double), no name hide happens
    virtual void f_int( int v ) {}

    virtual void f_double( double v ) {}
};

int main( int argc, char* argv[] )
{
    Derived d;
    d.f( 3 );
    d.f( 3.0 );
}
