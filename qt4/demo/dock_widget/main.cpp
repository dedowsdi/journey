#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QLabel>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QMainWindow mw;

  QLabel lbl;
  lbl.setText("central widget");
  mw.setCentralWidget(&lbl);

  QDockWidget dw0;
  dw0.setWindowTitle("dock0");
  mw.addDockWidget(Qt::LeftDockWidgetArea, &dw0);
  dw0.setBaseSize(500, 500);

  QDockWidget dw1;
  dw1.setWindowTitle("dock1");
  mw.addDockWidget(Qt::RightDockWidgetArea, &dw1);
  dw1.setGeometry(0, 0, 200, 200);

  QDockWidget dw2;
  dw2.setWindowTitle("dock2");
  mw.addDockWidget(Qt::TopDockWidgetArea, &dw2);

  QDockWidget dw3;
  dw3.setWindowTitle("dock3");
  mw.addDockWidget(Qt::BottomDockWidgetArea, &dw3);

  mw.show();

  app.exec();
  return 0;
}
