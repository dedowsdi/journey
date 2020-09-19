
class A
{
public:
    virtual A* clone() {return new A;}
    virtual A* create() {return new A;}
};

class B : public A
{
public:
    // you can return derived type, it's called covariant.
    virtual B* clone() {return new B;}
    virtual B* create() {return new B;}
};

int main( int argc, char *argv[] )
{

    return 0;
}
