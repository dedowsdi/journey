#include <QApplication>
#include <QUiLoader>
#include <QFile>
#include <QWidget>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QFile file("data/dynamic_dialog.ui");

  QUiLoader uiLoader;
  QWidget* dynamic_dialog = uiLoader.load(&file);
  if (dynamic_dialog) {
    dynamic_dialog->show(); 
  }
  
  return app.exec();
}
