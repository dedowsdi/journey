/*
 * Define an object that encapsulates how a set of objects interact. Mediator
 * promotes loose coupling by keeping objects from referring to each other
 * explicitly, and it lets you vary their interaction independently.
 */

#include <iostream>
#include <string>
#include <list>

class MediatorInterface;

class ColleagueInterface {
  std::string name;

public:
  ColleagueInterface(const std::string& newName) : name(newName) {}
  virtual ~ColleagueInterface(){}
  std::string getName() const { return name; }
  virtual void sendMessage(
    const MediatorInterface&, const std::string&) const = 0;
  virtual void receiveMessage(
    const ColleagueInterface*, const std::string&) const = 0;
};

class Colleague : public ColleagueInterface {
public:
  using ColleagueInterface::ColleagueInterface;
  virtual void sendMessage(
    const MediatorInterface&, const std::string&) const override;

private:
  virtual void receiveMessage(
    const ColleagueInterface*, const std::string&) const override;
};

class MediatorInterface {
private:
  std::list<ColleagueInterface*> colleagueList;

public:
  const std::list<ColleagueInterface*>& getColleagueList() const {
    return colleagueList;
  }
  virtual void distributeMessage(
    const ColleagueInterface*, const std::string&) const = 0;
  virtual void registerColleague(ColleagueInterface* colleague) {
    colleagueList.emplace_back(colleague);
  }
};

class Mediator : public MediatorInterface {
  virtual void distributeMessage(
    const ColleagueInterface* sender, const std::string& message) const override;
};

void Colleague::sendMessage(
  const MediatorInterface& mediator, const std::string& message) const {
  mediator.distributeMessage(this, message);
}

void Colleague::receiveMessage(
  const ColleagueInterface* sender, const std::string& message) const {
  std::cout << getName() << " received the message from " << sender->getName()
            << ": " << message << std::endl;
}

void Mediator::distributeMessage(
  const ColleagueInterface* sender, const std::string& message) const {
  for (const ColleagueInterface* x : getColleagueList())
    if (x != sender)  // Do not send the message back to the sender
      x->receiveMessage(sender, message);
}

int main() {
  Colleague *bob = new Colleague("Bob"), *sam = new Colleague("Sam"),
            *frank = new Colleague("Frank"), *tom = new Colleague("Tom");

  Colleague* staff[] = {bob, sam, frank, tom};
  Mediator mediatorStaff, mediatorSamsBuddies;
  for (Colleague* x : staff) mediatorStaff.registerColleague(x);
  bob->sendMessage(mediatorStaff, "I'm quitting this job!");

  mediatorSamsBuddies.registerColleague(frank);
  mediatorSamsBuddies.registerColleague(tom);  // Sam's buddies only
  sam->sendMessage(
    mediatorSamsBuddies, "Hooray!  He's gone!  Let's go for a drink, guys!");

  delete bob;
  delete sam;
  delete frank;
  delete tom;

  return 0;
}
