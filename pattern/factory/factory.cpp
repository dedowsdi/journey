/*
 *  A utility class that creates an instance of a class from a family of
 *  derived classes.
 */
#include <stdexcept>
#include <iostream>
#include <memory>

using namespace std;

// interface of piaaz
class Pizza {
public:
  virtual int getPrice() const = 0;
  virtual ~Pizza(){};
};

class HamAndMushroomPizza : public Pizza {
public:
  virtual int getPrice() const { return 850; };
};

class DeluxePizza : public Pizza {
public:
  virtual int getPrice() const { return 1050; };
};

class HawaiianPizza : public Pizza {
public:
  virtual int getPrice() const { return 1150; };
};

class PizzaFactory {
public:
  // you can use enum or string to distinguish specific pizza
  enum PizzaType { HamMushroom, Deluxe, Hawaiian };

  static Pizza* createPizza(PizzaType pizzaType) {
    switch (pizzaType) {
      case HamMushroom:
        return new HamAndMushroomPizza;
      case Deluxe:
        return new DeluxePizza;
      case Hawaiian:
        return new HawaiianPizza;
    }
    throw "invalid pizza type.";
  }
};

/*
* Create all available pizzas and print their prices
*/
void piaazInfo(PizzaFactory::PizzaType pizzatype) {
  Pizza* pizza = PizzaFactory::createPizza(pizzatype);
  cout << "Price of " << pizzatype << " is " << pizza->getPrice() << std::endl;
  delete pizza;
}

int main() {
  piaazInfo(PizzaFactory::HamMushroom);
  piaazInfo(PizzaFactory::Deluxe);
  piaazInfo(PizzaFactory::Hawaiian);
}
