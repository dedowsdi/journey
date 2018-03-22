#include "dialog.h"
#include <iostream>

//--------------------------------------------------------------------
Dialog::Dialog(QWidget *parent/* = 0*/):
  QDialog(parent)
{
  label = new QLabel();
  label->setText("hello");
  

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(emit_foo()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  layout = new QVBoxLayout;
  layout->addWidget(label);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

//--------------------------------------------------------------------
void Dialog::emit_foo()
{
  Node node;
  std::string s;
  emit foo(node, &node, node, s);
}

//--------------------------------------------------------------------
void Dialog::soo(const Node& node, const Node* pnode, Node n, std::string s)
{
  node.print();
}
