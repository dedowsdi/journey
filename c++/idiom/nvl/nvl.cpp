#include <iostream>


class Base
{
public:
  void init() // non virtual interface (template method)
  {
    init_stage_a();
    init_stage_b();
  }

private:
  virtual void init_stage_a(){}
  virtual void init_stage_b(){}
};

class Derived : public Base
{
public:

private:
  void init_stage_a() override{}
  void init_stage_b() override{}
};

int main(int argc, char *argv[])
{

  Derived d;
  d.init();
  
  return 0;
}
