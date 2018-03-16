#include "gotocell_dialog.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  //Ui::GoToCellDialog ui;
  //QDialog *dialog = new QDialog;
  //ui.setupUi(dialog);
  //dialog->show();
  
  GoToCellDialog  dialog;
  dialog.show();

  return app.exec();
}
