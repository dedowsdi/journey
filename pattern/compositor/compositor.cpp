// Composite lets clients treat individual objects and compositions of objects
// uniformly. The Composite pattern can represent both the conditions. In this
// pattern, one can develop tree structures for representing part-whole
// hierarchies.

#include <vector>
#include <iostream>   // std::cout
#include <memory>     // std::unique_ptr
#include <algorithm>  // std::for_each
using namespace std;

class Graphic {
public:
  virtual void print() const = 0;
  virtual ~Graphic() {}
};

class Ellipse : public Graphic {
public:
  void print() const { cout << "Ellipse \n"; }
};

class CompositeGraphic : public Graphic {
public:
  void print() const {
    for (Graphic *a : graphicList_) {
      a->print();
    }
  }

  void add(Graphic *aGraphic) { graphicList_.push_back(aGraphic); }

private:
  vector<Graphic *> graphicList_;
};

int main() {
  // Initialize four ellipses
  const unique_ptr<Ellipse> ellipse1(new Ellipse());
  const unique_ptr<Ellipse> ellipse2(new Ellipse());
  const unique_ptr<Ellipse> ellipse3(new Ellipse());
  const unique_ptr<Ellipse> ellipse4(new Ellipse());

  // Initialize three composite graphics
  const unique_ptr<CompositeGraphic> graphic(new CompositeGraphic());
  const unique_ptr<CompositeGraphic> graphic1(new CompositeGraphic());
  const unique_ptr<CompositeGraphic> graphic2(new CompositeGraphic());

  // Composes the graphics
  graphic1->add(ellipse1.get());
  graphic1->add(ellipse2.get());
  graphic1->add(ellipse3.get());

  graphic2->add(ellipse4.get());

  graphic->add(graphic1.get());
  graphic->add(graphic2.get());

  // Prints the complete graphic (four times the string "Ellipse")
  graphic->print();
  return 0;
}
