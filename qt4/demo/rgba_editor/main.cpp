#include <QApplication>
#include <QList>
#include "rgba_editor.h"
#include <iostream>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QList<quint32> colors;
  colors << 0xffff0000
         << 0xff00ff00
         << 0xff0000ff
         << 0x00007f7f;

  RgbaEditor editor(&colors);
  editor.show();

  app.exec();

  for (int i = 0; i < colors.count(); ++i) {
    std::cout << QString("0x%1").arg(colors[i],8, 16, QChar('0')).toStdString() << std::endl;
  }

  return 0;
}
