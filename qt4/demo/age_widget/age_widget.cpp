#include <QApplication>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>


int main(int argc, char *argv[])
{
  QApplication app(argc, argv); 

  QWidget* widget =  new QWidget;
  widget->setWindowTitle("Enter Your age");

  QSpinBox *spinBox = new QSpinBox();
  QSlider *slider = new QSlider(Qt::Horizontal);
  spinBox->setRange(0, 130);
  slider->setRange(0, 130);

  QObject::connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
  QObject::connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
  spinBox->setValue(35);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(spinBox);
  layout->addWidget(slider);

  widget->setLayout(layout);
  widget->show();

  app.exec();
  return 0;
}
