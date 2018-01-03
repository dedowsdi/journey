/*
 * The State Pattern allows an object to alter its behavior when its internal
 * state changes. The object will appear as having changed its class.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <memory>

enum Input { DUCK_DOWN, STAND_UP, JUMP, DIVE };

class Fighter;
class StandingState;
class JumpingState;
class DivingState;

class FighterState {
public:
  // static state for those has no properties
  static std::shared_ptr<StandingState> standing;
  static std::shared_ptr<DivingState> diving;
  virtual ~FighterState() = default;
  virtual void handleInput(Fighter&, Input) = 0;
  virtual void update(Fighter&) = 0;
};

class DuckingState : public FighterState {
private:
  int chargingTime;
  static const int FullRestTime = 5;

public:
  DuckingState() : chargingTime(0) {}
  virtual void handleInput(Fighter&, Input) override;
  virtual void update(Fighter&) override;
};

class StandingState : public FighterState {
public:
  virtual void handleInput(Fighter&, Input) override;
  virtual void update(Fighter&) override;
};

class JumpingState : public FighterState {
private:
  int jumpingHeight;

public:
  JumpingState() { jumpingHeight = std::rand() % 5 + 1; }
  virtual void handleInput(Fighter&, Input) override;
  virtual void update(Fighter&) override;
};

class DivingState : public FighterState {
public:
  virtual void handleInput(Fighter&, Input) override;
  virtual void update(Fighter&) override;
};

std::shared_ptr<StandingState> FighterState::standing(new StandingState);
std::shared_ptr<DivingState> FighterState::diving(new DivingState);

class Fighter {
private:
  std::string name;
  std::shared_ptr<FighterState> state;
  int fatigueLevel = std::rand() % 10;

public:
  Fighter(const std::string& newName)
      : name(newName), state(FighterState::standing) {}

  std::string getName() const { return name; }
  int getFatigueLevel() const { return fatigueLevel; }

  virtual void handleInput(Input input) {
    // delegate input handling to 'state'.
    state->handleInput(*this, input);
  }
  void changeState(std::shared_ptr<FighterState> newState) {
    state = newState;
    updateWithNewState();
  }
  void standsUp() { std::cout << getName() << " stands up." << std::endl; }
  void ducksDown() { std::cout << getName() << " ducks down." << std::endl; }
  void jumps() {
    std::cout << getName() << " jumps into the air." << std::endl;
  }
  void dives() {
    std::cout << getName() << " makes a dive attack in the middle of the jump!"
              << std::endl;
  }
  void feelsStrong() {
    std::cout << getName() << " feels strong!" << std::endl;
  }
  void changeFatigueLevelBy(int change) {
    fatigueLevel += change;
    std::cout << "fatigueLevel = " << fatigueLevel << std::endl;
  }

private:
  virtual void updateWithNewState() {
    // delegate updating to 'state'
    state->update(*this);
  }
};

void StandingState::handleInput(Fighter& fighter, Input input) {
  switch (input) {
    case STAND_UP:
      std::cout << fighter.getName() << " remains standing." << std::endl;
      break;
    case DUCK_DOWN:
      fighter.ducksDown();
      fighter.changeState(std::shared_ptr<DuckingState>(new DuckingState));
      break;
    case JUMP:
      fighter.jumps();
      fighter.changeState(std::shared_ptr<JumpingState>(new JumpingState));
      break;
    default:
      std::cout << "One cannot do that while standing.  " << fighter.getName()
                << " remains standing by default." << std::endl;
  }
}

void StandingState::update(Fighter& fighter) {
  if (fighter.getFatigueLevel() > 0) fighter.changeFatigueLevelBy(-1);
}

void DuckingState::handleInput(Fighter& fighter, Input input) {
  switch (input) {
    case STAND_UP:
      fighter.standsUp();
      fighter.changeState(FighterState::standing);
      break;
    case DUCK_DOWN:
      std::cout << fighter.getName() << " remains in ducking position, ";
      if (chargingTime < FullRestTime)
        std::cout << "recovering in the meantime." << std::endl;
      else
        std::cout << "fully recovered." << std::endl;
      return update(fighter);
    default:
      std::cout << "One cannot do that while ducking.  " << fighter.getName()
                << " remains in ducking position by default." << std::endl;
      update(fighter);
  }
}

void DuckingState::update(Fighter& fighter) {
  chargingTime++;
  std::cout << "Charging time = " << chargingTime << "." << std::endl;
  if (fighter.getFatigueLevel() > 0) fighter.changeFatigueLevelBy(-1);
  if (chargingTime >= FullRestTime && fighter.getFatigueLevel() <= 3)
    fighter.feelsStrong();
}

void JumpingState::handleInput(Fighter& fighter, Input input) {
  switch (input) {
    case DIVE:
      fighter.dives();
      fighter.changeState(FighterState::diving);
      break;
    default:
      std::cout << "One cannot do that in the middle of a jump.  "
                << fighter.getName()
                << " lands from his jump and is now standing again."
                << std::endl;
      fighter.changeState(FighterState::standing);
  }
}

void JumpingState::update(Fighter& fighter) {
  std::cout << fighter.getName() << " has jumped " << jumpingHeight
            << " feet into the air." << std::endl;
  if (jumpingHeight >= 3) fighter.changeFatigueLevelBy(1);
}

void DivingState::handleInput(Fighter& fighter, Input) {
  std::cout << "Regardless of what the user input is, " << fighter.getName()
            << " lands from his dive and is now standing again." << std::endl;
  fighter.changeState(FighterState::standing);
}

void DivingState::update(Fighter& fighter) { fighter.changeFatigueLevelBy(2); }

int main() {
  std::srand(std::time(nullptr));
  Fighter rex("Rex the Fighter"), borg("Borg the Fighter");
  std::cout << rex.getName() << " and " << borg.getName()
            << " are currently standing." << std::endl;
  int choice;
  auto chooseAction = [&choice](Fighter& fighter) {
    std::cout << std::endl
              << DUCK_DOWN + 1 << ") Duck down  " << STAND_UP + 1
              << ") Stand up  " << JUMP + 1 << ") Jump  " << DIVE + 1
              << ") Dive in the middle of a jump" << std::endl;
    std::cout << "Choice for " << fighter.getName() << "? ";
    std::cin >> choice;
    const Input input1 = static_cast<Input>(choice - 1);
    fighter.handleInput(input1);
  };
  while (true) {
    chooseAction(rex);
    chooseAction(borg);
  }
}
