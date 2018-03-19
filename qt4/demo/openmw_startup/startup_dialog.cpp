#include "startup_dialog.h"

//--------------------------------------------------------------------
StartupDialog::StartupDialog(QWidget* parent/* = 0*/):
  QDialog(parent)
{
  setupUi(this);

  connect(newGameButton, SIGNAL(clicked()), this, SIGNAL(createGame()));
  connect(newAddonButton, SIGNAL(clicked()), this, SIGNAL(createAddon()));
  connect(editContentButton, SIGNAL(clicked()), this, SIGNAL(loadDocument()));
  connect(configButton, SIGNAL(clicked()), this, SIGNAL(editConfig()));
}
