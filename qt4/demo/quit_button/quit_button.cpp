// Qt's widgets emit signals to indicate that a user action or a change of
// state has occurred
// QObject is base class of everything in QT
#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv); 

  QPushButton* btn = new QPushButton("quit");
  QObject::connect(btn, SIGNAL(clicked()), &app, SLOT(quit()));
  btn->show();

  return app.exec();
}
