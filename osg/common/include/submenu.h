#ifndef SUBMENU_H
#define SUBMENU_H

#include <osg/Object>

#include "menuelement.h"

namespace zxd {

class SubMenu;
class Menu;

class SubMenu : public MenuElement {
protected:
  MenuElements mChildren;  // both submenu and menu item resides here
  // this callback will be used if menu item has no select callback
  osg::ref_ptr<MenuElementSelectCallback> mUniformSelectCallback;

public:
  SubMenu();
  SubMenu(
    const SubMenu& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : MenuElement(copy, copyop) {}
  ~SubMenu();
  META_Object(zxd, SubMenu);

  MenuElement* getChildAt(unsigned int i);
  void removeChildAt(unsigned int i);

  void addItem(
    char c, const std::string& text, MenuElementSelectCallback* cb = 0);
  // use 1-9, a-z as select key
  MenuItem* addSeqItem(
    const std::string& text, MenuElementSelectCallback* cb = 0);
  SubMenu* addSeqSubMenu(
    const std::string& text, MenuElementSelectCallback* cb = 0);

  virtual void select();
  virtual bool accept(char c);
  virtual void setMenu(Menu* v);

  virtual SubMenu* asSubMenu() { return this; }

  osg::ref_ptr<MenuElementSelectCallback> getUniformSelectCallback() const {
    return mUniformSelectCallback;
  }
  void setUniformSelectCallback(osg::ref_ptr<MenuElementSelectCallback> v) {
    mUniformSelectCallback = v;
  }

  // full text, sub menut  + menu items
  std::string getContent();

protected:
  void addElement(MenuElement* element);
  // use 1-9, a-z as select key
  void addSeqElement(MenuElement* element);
};
}

#endif /* SUBMENU_H */
