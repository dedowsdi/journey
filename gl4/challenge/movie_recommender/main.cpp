#include <QApplication>
#include "MovieDialog.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MovieDialog dialog;
  dialog.exec();
  
  return app.exec();
}
