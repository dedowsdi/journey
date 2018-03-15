#ifndef GOTOCELL_DIALOG_H
#define GOTOCELL_DIALOG_H

#include <QDialog>
#include "ui_gotocell_dialog.h"

class GoToCellDialog : public QDialog, public Ui::GoToCellDialog
{
  Q_OBJECT
public:
  GoToCellDialog(QWidget *parent = 0);

private slots:
  // After creating the user interface, setupUi() will also automatically
  // connect any slots that follow the naming convention
  // on_objectName_signalName() to the corresponding objectName's signalName()
  // signal
  void on_lineEdit_textChanged();

};

#endif /* GOTOCELL_DIALOG_H */
