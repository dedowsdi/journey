#include "submenu.h"
#include "menu.h"
#include <sstream>
#include "menuitem.h"

namespace zxd {

//------------------------------------------------------------------------------
SubMenu::SubMenu() {}

//------------------------------------------------------------------------------
SubMenu::~SubMenu() {}

//------------------------------------------------------------------------------
MenuElement* SubMenu::getChildAt(unsigned int i) {
  if (i < mChildren.size()) return mChildren[i];
  return 0;
}

//------------------------------------------------------------------------------
void SubMenu::removeChildAt(unsigned int i) {
  if (i < mChildren.size()) {
    auto iter = mChildren.begin() + i;
    (*iter)->setParent(0);
    mChildren.erase(iter);
  }
}

//------------------------------------------------------------------------------
void SubMenu::addItem(
  char c, const std::string& text, MenuElementSelectCallback* cb /*= 0*/) {
  osg::ref_ptr<MenuItem> item = new MenuItem;
  item->setSelectKey(c);
  item->setSelectCallback(cb);
  addElement(item);
}

//------------------------------------------------------------------------------
MenuItem* SubMenu::addSeqItem(
  const std::string& text, MenuElementSelectCallback* cb /*= 0*/) {
  osg::ref_ptr<MenuItem> item = new MenuItem;
  item->setSelectCallback(cb);
  item->setText(text);
  addSeqElement(item);
  return item;
}

//------------------------------------------------------------------------------
SubMenu* SubMenu::addSeqSubMenu(
  const std::string& text, MenuElementSelectCallback* cb /*= 0*/) {
  osg::ref_ptr<SubMenu> sm = new SubMenu;
  sm->setSelectKey(mChildren.size());
  sm->setText(text);
  addSeqElement(sm);
  return sm;
}

//------------------------------------------------------------------------------
void SubMenu::select() {
  mMenu->setCurrentElement(this);
  if (mSelectCallback) {
    mSelectCallback->select(this);
  };
}

//------------------------------------------------------------------------------
bool SubMenu::accept(char c) {
  for (unsigned int i = 0; i < mChildren.size(); ++i) {
    MenuElement* element = mChildren[i];
    if (element->getSelectKey() == c) {
      element->select();
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
void SubMenu::setMenu(Menu* v) {
  mMenu = v;
  for (unsigned int i = 0; i < mChildren.size(); ++i) {
    mChildren[i]->setMenu(v);
  }
}

//------------------------------------------------------------------------------
std::string SubMenu::getContent() {
  std::stringstream ss;
  static std::string indent = "  ";
  ss << mText << "\n";

  for (unsigned int i = 0; i < mChildren.size(); ++i) {
    MenuElement* element = mChildren[i];
    ss << indent << element->getSelectKey() << " : " << element->getText()
       << "\n";
  }
  return ss.str();
}

//------------------------------------------------------------------------------
void SubMenu::addElement(MenuElement* element) {
#ifdef _DEBUG
  // key check
  for (unsigned int i = 0; i < mChildren.size(); ++i) {
    if (element->getSelectKey() == mChildren[i]->getSelectKey()) {
      std::stringstream ss;
      ss << element->getSelectKey() << " already used by "
         << mChildren[i]->getText();
      throw std::runtime_error(ss.str());
    }
  }
#endif

  mChildren.push_back(element);
  element->setParent(this);
  element->setMenu(mMenu);
}

//------------------------------------------------------------------------------
void SubMenu::addSeqElement(MenuElement* element) {
  // use 1-9, a-z as select key
  unsigned int index = mChildren.size() + 1;
  char c = index <= 8 ? ('0' + index) : 'a' + (index - 9);
  element->setSelectKey(c);
  addElement(element);
}
}
