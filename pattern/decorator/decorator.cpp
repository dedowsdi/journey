/*
 * The decorator pattern helps to attach additional behavior or responsibilities
 * to an object dynamically. Decorators provide a flexible alternative to
 * subclassing for extending functionality. This is also called “Wrapper”. If
 * your application does some kind of filtering, then Decorator might be good
 * pattern to consider for the job.
 */

#include <string>
#include <iostream>
using namespace std;

class Car  // Our Abstract base class
{
protected:
  string _str;

public:
  Car() { _str = "Unknown Car"; }

  virtual string getDescription() { return _str; }

  virtual double getCost() = 0;
  virtual ~Car() {}
};

class OptionsDecorator : public Car  // Decorator Base class
{
public:
  virtual string getDescription() = 0;
};

class CarModel1 : public Car {
public:
  CarModel1() { _str = "CarModel1"; }
  virtual double getCost() { return 31000.23; }
};

class Navigation : public OptionsDecorator {
  Car *_b;

public:
  Navigation(Car *b) { _b = b; }
  string getDescription() { return _b->getDescription() + ", Navigation"; }

  double getCost() { return 300.56 + _b->getCost(); }
};

class PremiumSoundSystem : public OptionsDecorator {
  Car *_b;

public:
  PremiumSoundSystem(Car *b) { _b = b; }
  string getDescription() {
    return _b->getDescription() + ", PremiumSoundSystem";
  }

  double getCost() { return 0.30 + _b->getCost(); }
};

class ManualTransmission : public OptionsDecorator {
  Car *_b;

public:
  ManualTransmission(Car *b) { _b = b; }
  string getDescription() {
    return _b->getDescription() + ", ManualTransmission";
  }

  double getCost() { return 0.30 + _b->getCost(); }
};

int main() {
  // Create our Car that we want to buy
  Car *b = new CarModel1();

  cout << "Base model of " << b->getDescription() << " costs $" << b->getCost()
       << "\n";

  // Who wants base model let's add some more features

  Car *nv = new Navigation(b);
  cout << nv->getDescription() << " will cost you $" << nv->getCost() << "\n";
  Car *pss = new PremiumSoundSystem(nv);
  Car *mt = new ManualTransmission(pss);
  cout << mt->getDescription() << " will cost you $" << mt->getCost() << "\n";

  delete b;
  delete nv;
  delete pss;
  delete mt;

  return 0;
}
