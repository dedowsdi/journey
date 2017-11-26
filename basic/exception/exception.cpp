#include <iostream>
#include <stdexcept>

template <typename T>
void _throw(T t) {
  try {
    // throw works with all kind of type.
    throw t;
  } catch (T e) {
    std::cout << e << std::endl;
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
    // rethrow the same exception
    throw;
  }
}

int main(int argc, char* argv[]) {
  _throw(0);
  _throw(0.0f);
  _throw(0.0);
  _throw(true);
  _throw(false);
  _throw("crash");

  try {
    _catch();
  } catch (const std::out_of_range& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
