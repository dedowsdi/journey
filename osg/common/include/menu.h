#ifndef MENU_H
#define MENU_H

#include <osg/Object>
#include "submenu.h"
#include "menuitem.h" //just for convinent
#include "menuelement.h"

namespace zxd {

class Menu;
class MenuItem;

class Menu : public osg::Object {
protected:
  osg::ref_ptr<SubMenu> mRoot;
  osg::ref_ptr<MenuElement> mCurrentElement;
  std::string mBackKeyString;  // eg: backspace, upper arrow

  // this callback will be used if both  menu item has no select callback and
  // it's parent menu has no uniform select callback
  osg::ref_ptr<MenuElementSelectCallback> mUniformSelectCallback;

public:
  Menu();
  Menu(const Menu& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::Object(copy, copyop) {}
  ~Menu();
  META_Object(zxd, Menu);

  // get current display content
  std::string getCurrentContent();
  // reset current menu to root, current item = 0
  void reset();

  bool accept(char c);
  bool back();

  osg::ref_ptr<SubMenu> getRoot() const { return mRoot; }
  void setRoot( osg::ref_ptr<SubMenu> v){mRoot = v;}

  std::string getBackKeyString() const { return mBackKeyString; }
  void setBackKeyString(std::string v) { mBackKeyString = v; }

  osg::ref_ptr<MenuElement> getCurrentElement() const {
    return mCurrentElement;
  }
  void setCurrentElement(osg::ref_ptr<MenuElement> v) { mCurrentElement = v; }

  osg::ref_ptr<MenuElementSelectCallback> getUniformSelectCallback() const { return mUniformSelectCallback; }
  void setUniformSelectCallback( osg::ref_ptr<MenuElementSelectCallback> v){mUniformSelectCallback = v;}

};
}

#endif /* MENU_H */
