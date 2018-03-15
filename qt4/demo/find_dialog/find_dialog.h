#ifndef FIND_DIALOG_H
#define FIND_DIALOG_H

#include <QDialog>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

// Qt automatically deletes child objects when the parent is destroyed, so you
// don't have to delete child objects.
class FindDialog : public QDialog
{
  // The Q_OBJECT macro at the beginning of the class definition is necessary
  // for all classes that define signals or slots
  // Classes that use the Q_OBJECT macro must have moc run on them
  Q_OBJECT

private:
  QLabel* label;
  QLineEdit* lineEdit;
  QCheckBox* caseCheckBox;
  QCheckBox* backwardCheckBox;
  QPushButton *findButton;
  QPushButton *closeButton;

public:
  FindDialog(QWidget* parent = 0);

signals:
  void findNext(const QString &str, Qt::CaseSensitivity cs);
  void findPrevious(const QString &str, Qt::CaseSensitivity cs);

private slots:
  void findClicked();
  void enableFindButton(const QString &text);
};

#endif /* FIND_DIALOG_H */
