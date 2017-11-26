/*
 * Abstract Factory patterns work around a super-factory which creates other
 * factories. This factory is also called as factory of factories.
 *
 * In Abstract Factory pattern an interface is responsible for creating a
 * factory of related objects without explicitly specifying their classes. Each
 * generated factory can give the objects as per the Factory pattern.
 *
 * In the following example, ChineseFactory create all kinds of chinese man and
 * woman inherits from Man and Woman, while AmericanFactory create all kinds of
 * american man and woman inherits from Man and Woman.
 */
#include <iostream>
#include <string>
#include <stdexcept>

class MainFactory;
class AbstractFactory;
class AmericanFactory;
class ChineseFactory;
class Man;
class Woman;
class ChineseTallMan;
class ChineseShortMan;
class ChineseTallWoman;
class ChineseShortWoman;
class AmericanTallMan;
class AmericanShortMan;
class AmericanTallWoman;
class AmericanShortWoman;

class Man {
public:
  virtual void talk() = 0;
  virtual ~Man() {}
};

class Woman {
public:
  virtual void talk() = 0;
  virtual ~Woman(){}
};

class ChineseTallMan : public Man {
  virtual void talk() {
    std::cout << "Chinese tall man is talking" << std::endl;
  }
};

class ChineseShortMan : public Man {
  virtual void talk() {
    std::cout << "Chinese short man is talking" << std::endl;
  }
};

class ChineseTallWoman : public Woman {
  virtual void talk() {
    std::cout << "Chinese tall woman is talking" << std::endl;
  }
};

class ChineseShortWoman : public Woman {
  virtual void talk() {
    std::cout << "Chinese short woman is talking" << std::endl;
  }
};

class AmericanTallMan : public Man {
  virtual void talk() {
    std::cout << "American tall man is talking" << std::endl;
  }
};

class AmericanShortMan : public Man {
  virtual void talk() {
    std::cout << "American short man is talking" << std::endl;
  }
};

class AmericanTallWoman : public Woman {
  virtual void talk() {
    std::cout << "American tall woman is talking" << std::endl;
  }
};

class AmericanShortWoman : public Woman {
  virtual void talk() {
    std::cout << "American short woman is talking" << std::endl;
  }
};

// factory interface
class AbstractFactory {
public:
  virtual Man* createMan(const std::string& type) = 0;
  virtual Woman* createWoman(const std::string& type) = 0;
  virtual ~AbstractFactory(){}
};

class AmericanFactory : public AbstractFactory {
public:
  virtual Man* createMan(const std::string& type) {
    if (type == "tall") {
      return new AmericanTallMan();
    } else if (type == "short") {
      return new AmericanShortMan();
    } else
      throw "unknown type";
  }
  virtual Woman* createWoman(const std::string& type) {
    if (type == "tall") {
      return new AmericanTallWoman();
    } else if (type == "short") {
      return new AmericanShortWoman();
    } else
      throw "unknown type";
  }
};

class ChineseFactory : public AbstractFactory {
public:
  virtual Man* createMan(const std::string& type) {
    if (type == "tall") {
      return new ChineseTallMan();
    } else if (type == "short") {
      return new ChineseShortMan();
    } else
      throw "unknown type";
  }
  virtual Woman* createWoman(const std::string& type) {
    if (type == "tall") {
      return new ChineseTallWoman();
    } else if (type == "short") {
      return new ChineseShortWoman();
    } else
      throw "unknown type";
  }
};

// factory of factory
class MainFactory {
public:
  AbstractFactory* getFactory(const std::string& type) {
    if (type == "American") {
      return new AmericanFactory();
    } else if (type == "Chinese") {
      return new ChineseFactory();
    } else {
      throw "unknown factory type";
    }
  }
};

int main(int argc, char* argv[]) {
  MainFactory mf;
  {
    AbstractFactory* cf = mf.getFactory("Chinese");
    Man* man0 = cf->createMan("tall");
    Man* man1 = cf->createMan("short");
    Woman* woman0 = cf->createWoman("tall");
    Woman* woman1 = cf->createWoman("short");
    man0->talk();
    man1->talk();
    woman0->talk();
    woman1->talk();
  }
  {
    AbstractFactory* cf = mf.getFactory("American");
    Man* man0 = cf->createMan("tall");
    Man* man1 = cf->createMan("short");
    Woman* woman0 = cf->createWoman("tall");
    Woman* woman1 = cf->createWoman("short");
    man0->talk();
    man1->talk();
    woman0->talk();
    woman1->talk();
  }

  return 0;
}
