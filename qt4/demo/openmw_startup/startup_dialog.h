#ifndef STARTUP_DIALOG_H
#define STARTUP_DIALOG_H
#include "ui_openmw_startup.h"

class StartupDialog : public QDialog, public Ui_StartupDialog{
  Q_OBJECT

public:

  StartupDialog(QWidget* parent = 0);

signals:
 
  void createGame();

  void createAddon();

  void loadDocument();

  void editConfig();

};

#endif /* STARTUP_DIALOG_H */
