#include "MainWindow.h"

#include "MainScene.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	auto mainScene = new MainScene(this);
	mainScene->setFocusPolicy(Qt::StrongFocus);
    setCentralWidget(mainScene);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
	{
	case Qt::Key_Escape:
		close();
		break;
	case Qt::Key_W:
	{
		// This will change the entire window, looks like qt draw mainwindow as a rect.
		//GLint polygonModes[2];
		//glGetIntegerv(GL_POLYGON_MODE, polygonModes);
		//auto nextMode = GL_POINT + (polygonModes[0] - GL_POINT + 1) % 3;
		//glPolygonMode(GL_FRONT_AND_BACK, nextMode);
		//break;
	}
	default:
		break;
	}

    QWidget::keyPressEvent(e);
}
