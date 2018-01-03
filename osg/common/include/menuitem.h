#ifndef MENUITEM_H
#define MENUITEM_H

#include <osg/Object>
#include "menuelement.h"

class Menu;

namespace zxd {

class MenuItem : public zxd::MenuElement {
public:
  MenuItem() {}
  MenuItem(
    const MenuItem& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : MenuElement(copy, copyop) {}
  ~MenuItem() {}
  META_Object(zxd, MenuItem);

  virtual void select();
  virtual bool accept(char c);

  virtual MenuItem* asMenuItem(){return this;}

  std::string getContent();
};
}

#endif /* MENUITEM_H */
