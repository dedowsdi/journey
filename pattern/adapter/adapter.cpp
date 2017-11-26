// Convert the interface of a class into another interface that clients expect.
// Adapter lets classes work together that couldn't otherwise because of
// incompatible interfaces.
#include <iostream>

class AnimalGuard {
public:
  ~AnimalGuard() {}
  virtual void guard() = 0;
};

class Dog : public AnimalGuard {
public:
  void guard() { std::cout << "A dog is guarding a door" << std::endl; }
};

class Cat {  // adaptee
public:
  void sleep() { std::cout << "A cat is sleepint at a door" << std::endl; }
};

class GuardAdapter : public AnimalGuard {  // adapter
protected:
  Cat* mCat;

public:
  GuardAdapter(Cat* cat) : mCat(cat) {}
  void guard() { mCat->sleep(); }
};

// need a animal guard
void guardTheDoor(AnimalGuard* guard) { guard->guard(); }

int main(int argc, char* argv[]) { 
  Dog d;
  guardTheDoor(&d);
  Cat c;
  GuardAdapter g(&c);
  guardTheDoor(&g);
}
