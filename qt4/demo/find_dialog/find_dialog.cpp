#include "find_dialog.h"

#include <QtGui> //a header file that contains the definition of Qt's GUI classes. 

//--------------------------------------------------------------------
FindDialog::FindDialog(QWidget* parent/* = 0*/):
  QDialog(parent)
{
  // The tr() function calls around the string literals mark them for
  // translation to other languages, it's declared in QObject and every subclass
  // that contains the Q_OBJECT macro
  label = new QLabel(tr("Find &what:")); // 1st character follow & will be shortcut
  lineEdit = new QLineEdit;
  label->setBuddy(lineEdit); // A buddy is a widget that accepts the focus when
                             // the label's shortcut key is pressed

  caseCheckBox = new QCheckBox(tr("Match &case"));
  backwardCheckBox = new QCheckBox(tr("Search &backward"));
  
  findButton = new QPushButton(tr("&Find"));
  findButton->setDefault(true); // default to this button, bind to enter
  findButton->setEnabled(false);

  closeButton = new QPushButton(tr("Close"));

  connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString &)));
  connect(findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
  // by default, dialog hid the widget from view(withoud deleting it).
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QHBoxLayout *topLeftLayout = new QHBoxLayout;
  topLeftLayout->addWidget(label);
  topLeftLayout->addWidget(lineEdit);

  QVBoxLayout *leftLayout = new QVBoxLayout;
  leftLayout->addLayout(topLeftLayout);
  leftLayout->addWidget(caseCheckBox);
  leftLayout->addWidget(backwardCheckBox);

  QVBoxLayout *rightLayout = new QVBoxLayout;
  rightLayout->addWidget(findButton);
  rightLayout->addWidget(closeButton);
  rightLayout->addStretch(); // spacer

  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->addLayout(leftLayout);
  mainLayout->addLayout(rightLayout);

  setLayout(mainLayout);

  setWindowTitle(tr("Find"));
  setFixedHeight(sizeHint().height()); // sizeHint returns a widget's "ideal" size
}

//--------------------------------------------------------------------
void FindDialog::findClicked()
{
  const QString& text = lineEdit->text();
  Qt::CaseSensitivity cs = caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

  if (backwardCheckBox->isChecked()) {
    emit findPrevious(text, cs);
  } else {
    emit findNext(text, cs);
  }
}

//--------------------------------------------------------------------
void FindDialog::enableFindButton(const QString &text)
{
  findButton->setEnabled(!text.isEmpty());
}
