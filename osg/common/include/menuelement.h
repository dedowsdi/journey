#ifndef MENUElEMENT_H
#define MENUElEMENT_H

#include <osg/Object>

namespace zxd {

class SubMenu;
class MenuItem;
class MenuElementSelectCallback;
class Menu;
class MenuElement;

typedef std::vector<osg::ref_ptr<MenuElement>> MenuElements;

class MenuElement : public osg::Object {
protected:
  char mSelectKey;
  MenuElement* mParent;
  Menu* mMenu;
  osg::ref_ptr<MenuElementSelectCallback> mSelectCallback;
  std::string mText;

public:
  MenuElement();
  MenuElement(const MenuElement& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::Object(copy, copyop) {}
  ~MenuElement();
  META_Object(zxd, MenuElement);

  char getSelectKey() const { return mSelectKey; }
  void setSelectKey(char v) { mSelectKey = v; }

  std::string getText() const { return mText; }
  void setText(std::string v) { mText = v; }

  Menu* getMenu() const { return mMenu; }
  virtual void setMenu(Menu* v) { mMenu = v; }

  virtual SubMenu* asSubMenu() { return 0; }
  virtual MenuItem* asMenuItem() { return 0; }

  MenuElement* getParent() const { return mParent; }
  void setParent(MenuElement* v) { mParent = v; }

  osg::ref_ptr<MenuElementSelectCallback> getSelectCallback() const {
    return mSelectCallback;
  }
  void setSelectCallback(osg::ref_ptr<MenuElementSelectCallback> v) {
    mSelectCallback = v;
  }

  virtual void select() {}
  virtual bool back();
  virtual bool accept(char c) { return false; }

  virtual std::string getContent() { return mText; }
};

class MenuElementSelectCallback : public osg::Object {
public:
  MenuElementSelectCallback() {}
  MenuElementSelectCallback(const MenuElementSelectCallback& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::Object(copy, copyop) {}
  ~MenuElementSelectCallback() {}
  META_Object(zxd, MenuElementSelectCallback);

  virtual void select(MenuElement* element) {}
};
}

#endif /* MENUElEMENT_H */
