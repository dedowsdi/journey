#ifndef SORT_DIALOG_H
#define SORT_DIALOG_H

#include <QDialog>
#include "ui_sort_dialog.h"

class SortDialog : public QDialog, public Ui::SortDialog
{
  Q_OBJECT

public:

  SortDialog(QWidget* parent = 0);

  void setColumnRange(QChar first, QChar last);

};
                   

#endif /* SORT_DIALOG_H */
