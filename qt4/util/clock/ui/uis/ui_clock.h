/********************************************************************************
** Form generated from reading UI file 'clock.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLOCK_H
#define UI_CLOCK_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *workEdit;
    QLabel *label_2;
    QLineEdit *restEdit;
    QPushButton *resetButton;
    QPushButton *exitButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(194, 111);
        gridLayout = new QGridLayout(Dialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        workEdit = new QLineEdit(Dialog);
        workEdit->setObjectName(QString::fromUtf8("workEdit"));

        gridLayout->addWidget(workEdit, 0, 1, 1, 2);

        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        restEdit = new QLineEdit(Dialog);
        restEdit->setObjectName(QString::fromUtf8("restEdit"));

        gridLayout->addWidget(restEdit, 1, 1, 1, 2);

        resetButton = new QPushButton(Dialog);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));

        gridLayout->addWidget(resetButton, 2, 0, 1, 2);

        exitButton = new QPushButton(Dialog);
        exitButton->setObjectName(QString::fromUtf8("exitButton"));

        gridLayout->addWidget(exitButton, 2, 2, 1, 1);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(workEdit);
        label_2->setBuddy(restEdit);
#endif // QT_NO_SHORTCUT

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "timer", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "work: ", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "rest:", 0, QApplication::UnicodeUTF8));
        resetButton->setText(QApplication::translate("Dialog", "reset", 0, QApplication::UnicodeUTF8));
        exitButton->setText(QApplication::translate("Dialog", "exit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLOCK_H
