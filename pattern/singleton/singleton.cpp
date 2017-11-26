#include <iostream>

class JonSnow {
public:
  static JonSnow& instance() {
    static JonSnow* snow = new JonSnow();
    return *snow;
  }

  void talk() { std::cout << "Who is my mother?" << std::endl; }

private:
  // private ctor, dtor
  JonSnow() {}
  //~JonSnow() {}  //you can un comment this if your single is in stack
  JonSnow(const JonSnow& snow); // no need to create body
  JonSnow& operator=(const JonSnow& snow); // no need to create body
};

int main(int argc, char* argv[]) {
  JonSnow::instance().talk();
  return 0;
}
