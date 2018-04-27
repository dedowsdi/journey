#include <QApplication>
#include <QVariant>
#include <QLabel>
#include <iostream>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QLabel* lbl = new QLabel();
  lbl->setText("label");

  // you must convert pointer to void* before store it into qvariant
  QVariant v = qVariantFromValue(static_cast<void*>(lbl));

  lbl = static_cast<QLabel*>(v.value<void*>());
  std::cout << lbl->text().toStdString()<< std::endl;

  return app.exec();
}
