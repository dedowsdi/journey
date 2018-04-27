#include <QApplication>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSizePolicy>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QDialog dialog;

  QPushButton* btn = new QPushButton;
  btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  btn->setMinimumSize(50, 50);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(btn);

  dialog.setLayout(layout);
  dialog.adjustSize();
  dialog.show();
  
  return app.exec();
}
