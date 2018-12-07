#include <iostream>
#include <stdexcept>
#include <vector>


class ExceptBase
{
public:
  ExceptBase()
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
  ExceptBase(const ExceptBase& e)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};

std::ostream& operator<<(std::ostream& os, const ExceptBase& eb)
{
  return os;
}

class ExceptDerived : public ExceptBase
{
public:
  ExceptDerived()
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  ExceptDerived(const ExceptDerived& e)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};

class Evil
{
public:
  // dont throw in destruction, either swallow it, eigher terminate the program
  ~Evil()
  {
    throw 5;
  }
};

void test_destruct_throw()
{
  try
  {
    {
      Evil e;
    }
  }
  catch(int i)
  {
    // no use, don't have chance to catch
    std::cout << "caught evil" << std::endl;
  }
}

template <typename T>
void _throw(const T& t) {
  try {
    // throw works with all kind of type. If t is not basic type, a copy
    // construct will be called, it's the caller the job to make sure static
    // type of T is the same as dyanmic type of T, otherwise, you lost derived
    // type info.
    throw t;
  } catch (T& e) {
    std::cout << e << std::endl;
  } catch(const char* e) {
    // very weird real type of t becomes const char[N]
    std::cout << "catch const char*" << std::endl;
  }
}

void _catch() {
  try {
    try {
      throw std::runtime_error("runtime");
    } catch (const std::runtime_error& e) {
      std::cout << e.what() << std::endl;
      throw std::out_of_range("out_of_range");
    } catch (const std::out_of_range& e) {
      // this catch block is useless because throw exits the whole compound
      // catch block
      std::cout << e.what() << std::endl;
    }

  } catch (const std::out_of_range& e) {
    std::cout << e.what() << std::endl;
    // rethrow the same exception(no object copied)
    throw;
  }
}

int main(int argc, char* argv[]) {
  _throw(0);
  _throw(0.0f);
  _throw(0.0);
  _throw(true);
  _throw(false);

  ExceptDerived ed;
  ExceptBase& eb = ed;
  _throw(eb); // derived part lost

  try {
    _catch();
  } catch (const std::out_of_range& e) {
    std::cout << e.what() << std::endl;
  }

  _throw("crash");
  test_destruct_throw();

  return 0;
}
