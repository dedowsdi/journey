#include <QApplication>
#include <QDialog>
#include <QPushButton>

#include "gotocell_dialog.h"

//--------------------------------------------------------------------
GoToCellDialog::GoToCellDialog(QWidget *parent/* = 0*/):
  QDialog(parent)
{
  setupUi(this);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

  QRegExp regExp("[A-Za-z][1-9][0-9]{0,2}");
  lineEdit->setValidator(new QRegExpValidator(regExp, this));

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

//--------------------------------------------------------------------
void GoToCellDialog::on_lineEdit_textChanged()
{
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(lineEdit->hasAcceptableInput());
}
