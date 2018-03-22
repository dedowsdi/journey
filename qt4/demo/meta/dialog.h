#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "node.h"
#include <string>


class Dialog : public QDialog
{
  Q_OBJECT

protected:
  QLabel* label;
  QVBoxLayout* layout;
  QDialogButtonBox* buttonBox;

public:
  Dialog(QWidget *parent = 0);

signals:
  void foo(const Node& node, const Node* pnode, Node n, std::string s);


protected slots:

  void soo(const Node& node, const Node* pnode, Node n, std::string s);
  void emit_foo();
};


#endif /* DIALOG_H */
