#include "hex_spinbox.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  HexSpinBox spinBox;
  spinBox.show();

  return app.exec();
}
