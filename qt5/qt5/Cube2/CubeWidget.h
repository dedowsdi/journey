#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class CubeWidget : public QOpenGLWidget, public QOpenGLFunctions
{
	Q_OBJECT

public:
	CubeWidget(QWidget* parent = nullptr);
	~CubeWidget();

	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;

public slots:
	void updateScene();

private:
	GLfloat _yaw = 0;
	GLfloat _pitch = 0;
	GLfloat _row = 0;

	QTimer* _timer = 0;
};

