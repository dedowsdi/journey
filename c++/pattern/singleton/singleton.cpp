#include <iostream>

class JonSnow {
public:
  static JonSnow& instance() {
    static JonSnow* snow = new JonSnow();
    return *snow;
  }

  void talk() { std::cout << "Who is my mother?" << std::endl; }
  //~JonSnow() {}  //you can un comment this if your single is in stack
  JonSnow(const JonSnow& snow) = delete;
  JonSnow& operator=(const JonSnow& snow) = delete;

private:
  // private ctor, dtor
  JonSnow() {}
};

int main(int argc, char* argv[]) {
  JonSnow::instance().talk();
  return 0;
}
