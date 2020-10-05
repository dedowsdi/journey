#include "CubeWidget.h"

#include <QDebug>
#include <GL/GLU.h>
#include <QTimer>
#include <QVector3D>
#include <QMatrix4x4>

CubeWidget::CubeWidget(QWidget* parent) :
	QOpenGLWidget(parent)
{
	_timer = new QTimer(parent);
	_timer->start(16.666f);
	_timer->setSingleShot(false);
	connect(_timer, SIGNAL(timeout()), this, SLOT(updateScene()));
}

CubeWidget::~CubeWidget()
{
}

void CubeWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(0, 0.5f, 0.5f, 1.0f);
}

void renderQuad(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3)
{
	glVertex3f(p0.x(), p0.y(), p0.z());
	glVertex3f(p1.x(), p1.y(), p1.z());
	glVertex3f(p2.x(), p2.y(), p2.z());
	glVertex3f(p3.x(), p3.y(), p3.z());
}

void CubeWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(_yaw, 0, 0, 1);
	glRotatef(_pitch, 1, 0, 0);
	glRotatef(_row, 0, 1, 0);

	glEnable(GL_DEPTH_TEST);

	// top
	QVector3D p0(1, 1, 1);
	QVector3D p1(-1, 1, 1);
	QVector3D p2(-1, -1, 1);
	QVector3D p3(1, -1, 1);

	// bottom
	QVector3D p4(1, -1, -1);
	QVector3D p5(-1, -1, -1);
	QVector3D p6(-1, 1, -1);
	QVector3D p7(1, 1, -1);

	glBegin(GL_QUADS);

	// top, front, right
	glColor3f(1, 0, 0);
	renderQuad(p0, p1, p2, p3);
	glColor3f(0, 1, 0);
	renderQuad(p2, p5, p4, p3);
	glColor3f(0, 0, 1);
	renderQuad(p4, p7, p0, p3);

	// bottom, back, left
	glColor3f(0.3, 0, 0);
	renderQuad(p4, p5, p6, p7);
	glColor3f(0, 0.3, 0);
	renderQuad(p0, p7, p6, p1);
	glColor3f(0, 0, 0.3);
	renderQuad(p2, p1, p6, p5);

	glEnd();

	glPopMatrix();
}

void CubeWidget::resizeGL(int w, int h)
{
	qDebug() << "Resize to " << w << " " << h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, static_cast<GLfloat>(w) / h, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5, -5, 5, 0, 0, 0, 0, 0, 1);
}

void CubeWidget::updateScene()
{
	_yaw += 0.3;
	_pitch += 0.2;
	_row += 0.1;
	update();
}
