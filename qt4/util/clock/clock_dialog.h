#ifndef CLOCK_DIALOG_H
#define CLOCK_DIALOG_H

#include <QDialog>
#include <QTimer>
#include <QTime>
#include "ui/uis/ui_clock.h"

class ClockDialog : public QDialog
{

private:

  Q_OBJECT
  Ui_Dialog mUi;
  QTimer mWorkClock, mRestClock, mSecondClock;
  QTime mTime;

public:
  ClockDialog(QWidget *parent = 0);

protected slots:

  void onWork();
  void onRest();
  void onReset();
  void onSecond();
  void onExit();

};

#endif /* CLOCK_DIALOG_H */
