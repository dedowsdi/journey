#include "startup_dialog.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  StartupDialog dialog;
  dialog.show();

  return app.exec();
}
