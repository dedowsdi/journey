#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  // you can use html format in label
  QLabel *label = new QLabel("<h2><i>hello <font color=red>qute</font></i>");
  label->show();
  return app.exec();
}
