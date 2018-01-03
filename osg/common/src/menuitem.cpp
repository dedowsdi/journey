#include "menuitem.h"
#include "menu.h"

namespace zxd {

//------------------------------------------------------------------------------
void MenuItem::select() {
  mMenu->setCurrentElement(this);

  MenuElementSelectCallback* cb = 0;

  //  mSelectCallback > parent sub menu uniform select callback > menu uniform
  //  selectcallback
  if (mSelectCallback)
    cb = mSelectCallback;
  else if (mParent) {
    cb = mParent->asSubMenu()->getUniformSelectCallback();
  }

  if (!cb) cb = mMenu->getUniformSelectCallback();

  if (cb) cb->select(this);
}

//------------------------------------------------------------------------------
bool MenuItem::accept(char c) {
  if (c != mSelectKey) return false;
  select();
  return true;
}

//------------------------------------------------------------------------------
std::string MenuItem::getContent() { return mText + "\n"; }
}
