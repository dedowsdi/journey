#include "menuelement.h"
#include "menu.h"

namespace zxd {

//------------------------------------------------------------------------------
MenuElement::MenuElement() : mSelectKey('@'), mParent(0), mMenu(0) {}

//------------------------------------------------------------------------------
MenuElement::~MenuElement() {}

//------------------------------------------------------------------------------
bool MenuElement::back() {
  if (mParent) {
    mParent->select();
    return true;
  }
  return false;
}
}
