/*
 * The Proxy Pattern will provide an object a surrogate or placeholder for
 * another object to control access to it. This placeholder object is called
 * the “Proxy” for the real object.
 *
 * Proxy and the real object have the same interface.
 *
 * Proxy will be used to forward request to the real object, some times it also
 * manage life time of the real object.
 */

#include <iostream>

using namespace std;

class Subject {
public:
  virtual void request() = 0;
  virtual ~Subject() {}
};

class RealSubject : public Subject {
public:
  void request() { cout << "RealSubject.request()" << endl; }
};

class Proxy : public Subject {
private:
  Subject* realSubject;

public:
  Proxy() : realSubject(new RealSubject()) {}
  ~Proxy() { delete realSubject; }
  // Forward calls to the RealSubject:
  void request() { realSubject->request(); }
};

int main() {
  Proxy p;
  p.request();
}
