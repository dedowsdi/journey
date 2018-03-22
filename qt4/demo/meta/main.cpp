/* Declare new types with Q_DECLARE_METATYPE() to make them available to
 * QVariant and other template-based functions. Call qRegisterMetaType() to
 * make type available to non-template based functions, such as the queued
 * signal and slot connections.
 *
 * To use the type T in QVariant, using Q_DECLARE_METATYPE() is sufficient. To
 * use the type T in queued signal and slot connections, qRegisterMetaType<T>()
 * must be called before the first connection is established.
 */

#include "node.h"
#include <QMetaType>
#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{ 
  QApplication app(argc, argv);
   // is this necessary? comment it, dialog  signal slot still works
  //qRegisterMetaType<Node>("node");

  void *p = QMetaType::construct(qMetaTypeId<Node>());
  Node* node = static_cast<Node*>(p);
  node->print();
  QMetaType::destroy(qMetaTypeId<Node>(), p);

  Dialog dialog;
  dialog.show();
  
  return app.exec();
}

