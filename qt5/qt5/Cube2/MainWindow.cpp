#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    _cubeWidget = new CubeWidget;
    setCentralWidget(_cubeWidget);
}
