#include "menu.h"
namespace zxd {

//------------------------------------------------------------------------------
Menu::Menu() {
  mRoot = new zxd::SubMenu;
  mRoot->setText("default");
  mRoot->setMenu(this);
  mCurrentElement = mRoot;
}

//------------------------------------------------------------------------------
Menu::~Menu() {}

//------------------------------------------------------------------------------
std::string Menu::getCurrentContent() {
  return mCurrentElement->getContent() + mBackKeyString + " : return\n";
}

//------------------------------------------------------------------------------
void Menu::reset() { mCurrentElement = mRoot; }

//------------------------------------------------------------------------------
bool Menu::accept(char c) {
  // do nothing if current elemnt is in an menu item
  if (mCurrentElement->asMenuItem()) return false;

  return mCurrentElement->accept(c);
}

//------------------------------------------------------------------------------
bool Menu::back() { return mCurrentElement->back(); }
}
