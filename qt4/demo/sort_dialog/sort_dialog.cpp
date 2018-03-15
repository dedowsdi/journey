#include "sort_dialog.h"
#include <QtGui>

//--------------------------------------------------------------------
SortDialog::SortDialog(QWidget* parent/* = 0*/):
  QDialog(parent)
{
  setupUi(this);

  secondaryGroupBox->hide();
  tertiaryGroupBox->hide();
  // make dialog non-resiable by user The layout then takes over the
  // responsibility for resizing, and resizes the dialog automatically when
  // child widgets are shown or hidden, ensuring that the dialog is always
  // displayed at its optimal size..
  layout()->setSizeConstraint(QLayout::SetFixedSize);

  setColumnRange('A', 'Z');
}

//--------------------------------------------------------------------
void SortDialog::setColumnRange(QChar first, QChar last)
{
  primaryColumnCombo->clear();
  secondaryColumnCombo->clear();
  tertiaryColumnCombo->clear();

  secondaryColumnCombo->addItem(tr("None"));
  tertiaryColumnCombo->addItem(tr("None"));
  // secondary and tertiary combo contains "None", it's larger than primary
  // combobox which contains only single-letter entries. 
  primaryColumnCombo->setMinimumSize(secondaryColumnCombo->sizeHint());

  QChar ch = first;
  while (ch <= last) {
    primaryColumnCombo->addItem(QString(ch));
    secondaryColumnCombo->addItem(QString(ch));
    tertiaryColumnCombo->addItem(QString(ch));
    ch = ch.unicode() + 1;
  }
}
