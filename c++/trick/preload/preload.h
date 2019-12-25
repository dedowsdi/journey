
// interpose osg::SomeClass::foo(), note that you can not use in-class
// definition.
namespace osg
{

class SomeClass
{
public:
    void foo();
};

}

