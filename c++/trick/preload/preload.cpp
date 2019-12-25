#include "preload.h"
#include <dlfcn.h>
#include <cstring>
#include <iostream>

// interpose rand
using rand_func = int (*)();
rand_func _orig_rand = 0;

int rand()
{
    std::cout << "hijack rand, do whatever i want" << std::endl;

    // RTLD_NEXT told dl to look for symbol in other so.
    if (_orig_rand == 0)
    {
        auto func = dlsym(RTLD_NEXT, "rand");
        if (func == 0)
        {
            std::cout << dlerror() << std::endl;
            exit(1);
        }
        memcpy(&_orig_rand, &func, sizeof(func));
    }

    return _orig_rand();
}

using foo_func = void (*)();
foo_func _orig_foo = 0;

// interpose osg::SomeClass::foo()
namespace osg
{
    // note that you can't use in-class definition!!!
    void SomeClass::foo()
    {
        std::cout << "hijack foo, do whatever i want" << std::endl;

        // use mangled name to get original method, you can get mangled name by
        // readelf -s *.cpp.o , the namespace and class part doesn't change?
        // looks like 3 means 3 characters for osg, 9 means 9 characters for
        // SomeClass, the last 3 means 3 characters for foo?
        if (_orig_foo == 0)
        {
            auto func = dlsym(RTLD_NEXT, "_ZN3osg9SomeClass3fooEv");
            if (func == 0)
            {
                std::cout << dlerror() << std::endl;
                exit(1);
            }
            memcpy(&_orig_foo, &func, sizeof(func));
        }

        _orig_foo();
    }
};
