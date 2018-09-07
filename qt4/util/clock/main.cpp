#include "clock_dialog.h"

#include <QApplication>
#include <QSound>
#include <iostream>
#include <phonon/phonon>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  ClockDialog dialog;
  dialog.show();

  return app.exec();
}
