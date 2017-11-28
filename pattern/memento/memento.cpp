/*
 * Without violating encapsulation the Memento Pattern will capture and
 * externalize an object’s internal state so that the object can be restored to
 * this state later.  Best Use case is 'Undo-Redo' in an editor.
 *
 * The Originator (the object to be saved) creates a snap-shot of itself as a
 * Memento object, and passes that reference to the Caretaker object. The
 * Caretaker object keeps the Memento until such a time as the Originator may
 * want to revert to a previous state as recorded in the Memento object.
 *
 */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

const std::string NAME = "Object";

template <typename T>
std::string toString(const T& t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

class Memento;

class Object {
private:
  int value;
  std::string name;
  double decimal;  // and suppose there are loads of other data members
public:
  Object(int newValue)
      : value(newValue),
        name(NAME + toString(value)),
        decimal((float)value / 100) {}

  void doubleValue() {
    value = 2 * value;
    name = NAME + toString(value);
    decimal = (float)value / 100;
  }
  void increaseByOne() {
    value++;
    name = NAME + toString(value);
    decimal = (float)value / 100;
  }

  int getValue() const { return value; }
  std::string getName() const { return name; }
  double getDecimal() const { return decimal; }

  Memento* createMemento() const;
  void reinstateMemento(Memento* mem);
};

class Memento {
private:
  Object object;

public:
  Memento(const Object& obj) : object(obj) {}
  // want a snapshot of Object itself because of its many data members
  Object snapshot() const { return object; }
};

Memento* Object::createMemento() const { return new Memento(*this); }

void Object::reinstateMemento(Memento* mem) { *this = mem->snapshot(); }

class Command {
private:
  typedef void (Object::*Action)();
  Object* receiver;
  Action action;
  static std::vector<Command*> commandList;
  static std::vector<Memento*> mementoList;  // the same size as command list
  static int numCommands;                    // number of commands undoable
  static int maxCommands;                    // the last executed commands index

public:
  Command(Object* newReceiver, Action newAction)
      : receiver(newReceiver), action(newAction) {}

  void execute() {
    if (mementoList.size() < numCommands + 1)
      mementoList.resize(numCommands + 1);

    if (commandList.size() < numCommands + 1)
      commandList.resize(numCommands + 1);

    // saves the last command
    commandList[numCommands] = this;

    // if (numCommands > maxCommands) maxCommands = numCommands;

    // clean history tree everytime a command is executed. Which means;
    //   undo
    //   undo
    //   undo
    //   execute()
    //   redo() //nothing will happen
    for (int i = numCommands; i <= maxCommands; ++i) {
      delete mementoList[i];
    }
    maxCommands = numCommands;

    // saves the last value
    mementoList[numCommands] = receiver->createMemento();

    numCommands++;
    (receiver->*action)();
  }

  static void undo() {
    if (numCommands == 0) {
      std::cout << "There is nothing to undo at this point." << std::endl;
      return;
    }
    commandList[numCommands - 1]->receiver->reinstateMemento(
      mementoList[numCommands - 1]);
    numCommands--;
  }
  void static redo() {
    if (numCommands > maxCommands) {
      std::cout << "There is nothing to redo at this point." << std::endl;
      return;
    }
    Command* commandRedo = commandList[numCommands];
    (commandRedo->receiver->*(commandRedo->action))();
    numCommands++;
  }

  static std::vector<Memento*>& getMementoes() { return mementoList; }
};

std::vector<Command*> Command::commandList;
std::vector<Memento*> Command::mementoList;
int Command::numCommands = 0;
int Command::maxCommands = 0;

int main() {
  int i;
  std::cout << "Please enter an integer: ";
  std::cin >> i;
  Object* object = new Object(i);

  Command* commands[3];
  commands[1] = new Command(object, &Object::doubleValue);
  commands[2] = new Command(object, &Object::increaseByOne);

  std::cout << "0.Exit,  1.Double,  2.Increase by one,  3.Undo,  4.Redo: ";
  std::cin >> i;

  while (i != 0) {
    if (i == 3)
      Command::undo();
    else if (i == 4)
      Command::redo();
    else if (i > 0 && i <= 2)
      commands[i]->execute();
    else {
      std::cout << "Enter a proper choice: ";
      std::cin >> i;
      continue;
    }
    std::cout << "   " << object->getValue() << "  " << object->getName()
              << "  " << object->getDecimal() << std::endl;
    std::cout << "0.Exit,  1.Double,  2.Increase by one,  3.Undo,  4.Redo: ";
    std::cin >> i;
  }

  delete commands[1];
  delete commands[2];

  std::vector<Memento*> mementoes = Command::getMementoes();
  std::for_each(mementoes.begin(), mementoes.end(),
    [&](decltype(*mementoes.begin()) v) { delete v; });
}
