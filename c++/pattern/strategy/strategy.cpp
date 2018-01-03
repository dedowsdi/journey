/*
 * Defines a family of algorithms, encapsulates each one, and make them
 * interchangeable. Strategy lets the algorithm vary independently from clients
 * who use it.
 */

#include <iostream>
using namespace std;

class StrategyInterface {
public:
  virtual void execute() const = 0;
};

class ConcreteStrategyA : public StrategyInterface {
public:
  void execute() const override {
    cout << "Called ConcreteStrategyA execute method" << endl;
  }
};

class ConcreteStrategyB : public StrategyInterface {
public:
  void execute() const override {
    cout << "Called ConcreteStrategyB execute method" << endl;
  }
};

class ConcreteStrategyC : public StrategyInterface {
public:
  void execute() const override {
    cout << "Called ConcreteStrategyC execute method" << endl;
  }
};

class Context {
private:
  StrategyInterface *strategy_;

public:
  explicit Context(StrategyInterface *strategy) : strategy_(strategy) {}

  void set_strategy(StrategyInterface *strategy) { strategy_ = strategy; }

  void execute() const { strategy_->execute(); }
};

int main(int argc, char *argv[]) {
  ConcreteStrategyA concreteStrategyA;
  ConcreteStrategyB concreteStrategyB;
  ConcreteStrategyC concreteStrategyC;

  Context contextA(&concreteStrategyA);
  Context contextB(&concreteStrategyB);
  Context contextC(&concreteStrategyC);

  contextA.execute();  // output: "Called ConcreteStrategyA execute method"
  contextB.execute();  // output: "Called ConcreteStrategyB execute method"
  contextC.execute();  // output: "Called ConcreteStrategyC execute method"

  contextA.set_strategy(&concreteStrategyB);
  contextA.execute();  // output: "Called ConcreteStrategyB execute method"
  contextA.set_strategy(&concreteStrategyC);
  contextA.execute();  // output: "Called ConcreteStrategyC execute method"

  return 0;
}
