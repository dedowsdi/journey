#include "sort_dialog.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  SortDialog dialog;
  dialog.show();

  return app.exec();
}
