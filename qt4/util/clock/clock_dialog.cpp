#include "clock_dialog.h"
#include <iostream>
#include <phonon/phonon>
#include <cmath>

//--------------------------------------------------------------------
ClockDialog::ClockDialog(QWidget *parent/* = 0*/)
{
  mUi.setupUi(this);

  mUi.workEdit->setText("45");
  mUi.restEdit->setText("10");


  QObject::connect(&mWorkClock, SIGNAL(timeout()), this, SLOT(onWork()));
  QObject::connect(&mRestClock, SIGNAL(timeout()), this, SLOT(onRest()));
  QObject::connect(&mSecondClock, SIGNAL(timeout()), this, SLOT(onSecond()));

  QObject::connect(mUi.resetButton, SIGNAL(clicked()), this, SLOT(onReset()));
  QObject::connect(mUi.exitButton, SIGNAL(clicked()), this, SLOT(onExit()));

  mUi.resetButton->setText("start");
  onReset();
}

//--------------------------------------------------------------------
void ClockDialog::onWork()
{
  Phonon::MediaObject* bell = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource("bell.wave"));
  bell->play();

  mUi.workEdit->setText(QString::number(mWorkClock.interval() / (1000 * 60.0)));
  mTime.restart();
  mRestClock.start();
}

//--------------------------------------------------------------------
void ClockDialog::onRest()
{
  Phonon::MediaObject* bell = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource("bell.wave"));
  bell->play();

  mUi.restEdit->setText(QString::number(mRestClock.interval() / (1000 * 60.0)));
  mTime.restart();
  mWorkClock.start();
}

//--------------------------------------------------------------------
void ClockDialog::onReset()
{
  if(mUi.resetButton->text() == "start")
  {
    mUi.workEdit->setEnabled(false);
    mUi.restEdit->setEnabled(false);
    mUi.resetButton->setText("reset");

    mWorkClock.setInterval(mUi.workEdit->text().toFloat() * 60 * 1000);
    mWorkClock.setSingleShot(true);
    mRestClock.setInterval(mUi.restEdit->text().toFloat() * 60 * 1000);
    mRestClock.setSingleShot(true);

    std::cout << "reset work interval to " << mWorkClock.interval()/1000.0 << " second" << std::endl;
    std::cout << "reset rest interval to " << mRestClock.interval()/1000.0 << " second" << std::endl;
    
    mRestClock.stop();
    mWorkClock.start();
    mSecondClock.start();

    mTime.start();
  }
  else
  {
    mUi.workEdit->setEnabled(true);
    mUi.restEdit->setEnabled(true);
    mUi.resetButton->setText("start");

    mUi.workEdit->setText(QString::number(mWorkClock.interval() / (1000 * 60.0)));
    mUi.restEdit->setText(QString::number(mRestClock.interval() / (1000 * 60.0)));

    mRestClock.stop();
    mWorkClock.stop();
    mSecondClock.stop();
  }

}

//--------------------------------------------------------------------
void ClockDialog::onSecond()
{
  if(mWorkClock.isActive())
  {
    int t = mWorkClock.interval() - mTime.elapsed();
    mUi.workEdit->setText(QString::number(t/60000) + ":" + QString::number((t%60000) / 1000));
  }

  if(mRestClock.isActive())
  {
    int t = mRestClock.interval() - mTime.elapsed();
    mUi.restEdit->setText(QString::number(t/60000) + ":" + QString::number((t%60000) / 1000));
  }
}

//--------------------------------------------------------------------
void ClockDialog::onExit()
{
  QApplication::quit();
}

