#include <iostream>

class A
{
};

A& byPointer()
{
    // p is never deleted, the underlying operating system will automatically
    // reclaim all the memory in program's heap when it exists.
    static auto p = new A;
    return *p;
}

A& byObject()
{
    // If you use this approach, you will suffer static deinitialization fiasco.
    // You must make sure it's not destructed before any other object that use
    // it.
    static A a;
    return a;
}


int main( int argc, char *argv[] )
{
    
    return 0;
}
