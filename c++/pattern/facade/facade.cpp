/*
 * The Facade Pattern hides the complexities of the system by providing an
 * interface to the client from where the client can access the system on an
 * unified interface. Facade defines a higher-level interface that makes the
 * subsystem easier to use. For instance making one class method perform a
 * complex process by calling several other classes.
 */
#include <string>
#include <iostream>

using namespace std;

class Alarm {
public:
  void alarmOn() { cout << "Alarm is on and house is secured" << endl; }

  void alarmOff() {
    cout << "Alarm is off and you can go into the house" << endl;
  }
};

class Ac {
public:
  void acOn() { cout << "Ac is on" << endl; }

  void acOff() { cout << "AC is off" << endl; }
};

class Tv {
public:
  void tvOn() { cout << "Tv is on" << endl; }

  void tvOff() { cout << "TV is off" << endl; }
};

class HouseFacade {
  Alarm alarm;
  Ac ac;
  Tv tv;

public:
  HouseFacade() {}

  void goToWork() {
    ac.acOff();
    tv.tvOff();
    alarm.alarmOn();
  }

  void comeHome() {
    alarm.alarmOff();
    ac.acOn();
    tv.tvOn();
  }
};

int main() {
  HouseFacade hf;

  // Rather than calling 100 different on and off functions thanks to facade I
  // only have 2 functions...
  hf.goToWork();
  hf.comeHome();
}
