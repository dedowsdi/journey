#include "UVSphere.h"

#include <QtMath>
#include <QVector3D>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>
#include "mutil.h"

#define BUFFER_OFFSET(bytes) (reinterpret_cast<GLubyte*>(0) + bytes)

void UVSphere::draw()
{
	if (!_support)
	{
		return;
	}

	if(!_vao.isCreated())
	{
		init();
		if (!_support)
		{
			return;
		}
	}

	glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	_vao.bind();
	_ibo.bind();
	glDrawElements(GL_TRIANGLE_STRIP, _stacks * (_slices + 1) * 2 + _stacks, GL_UNSIGNED_INT, 0);
	glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
}

namespace
{
struct VertexData
{
	QVector3D vertex;
	QVector3D normal;
	QVector2D texcoord;
};

}

void UVSphere::init()
{
	auto ctx = QOpenGLContext::currentContext();
	auto glf = ctx->functions();
	_support = ctx->hasExtension(QByteArrayLiteral("GL_ARB_ES3_compatibility"));

	if (!_support)
	{
		qWarning() << "Failed to init sphere, require ARB_ES3_compatibility";
		return;
	}

	_vao.create();
	_vao.bind();

	_vbo.create();
	_vbo.bind();

	_ibo.create();
	_ibo.bind();

	// init sphere points
	auto points = createSpherePoints(_radius, _slices, _stacks);
	auto texcoords = mutil::createGridTexcoordsStackByStack(_stacks, _slices, QVector2D(0, 1), QVector2D(1, 0));
	auto indices = mutil::createGridIndices(_stacks, _slices);

	assert(indices.size() == _stacks * (_slices + 1) * 2 + _stacks);

	std::vector<VertexData> vertices;
	vertices.resize(points.size());
	for (auto i = 0; i < vertices.size(); i++)
	{
		vertices[i].vertex = points[i];
		vertices[i].normal = points[i];
		vertices[i].normal.normalize();
		vertices[i].texcoord = texcoords[i];
	}

	_vbo.allocate(vertices.data(), vertices.size() * sizeof(VertexData));
	_ibo.allocate(indices.data(), indices.size() * sizeof(GLuint));

	auto stride = sizeof(VertexData);
	glf->glEnableVertexAttribArray(0);
	glf->glVertexAttribPointer(0, 3, GL_FLOAT, 0, stride, BUFFER_OFFSET(0));

	auto normalIndex = _legacy ? 2 : 1;
	glf->glEnableVertexAttribArray(normalIndex);
	glf->glVertexAttribPointer(normalIndex, 3, GL_FLOAT, 0, stride, BUFFER_OFFSET(sizeof(QVector3D)));

	auto texcoordIndex = _legacy ? 8 : 2;
	glf->glEnableVertexAttribArray(texcoordIndex);
	glf->glVertexAttribPointer(texcoordIndex, 2, GL_FLOAT, 0, stride, BUFFER_OFFSET(sizeof(QVector3D) * 2));
}

std::vector<QVector3D> UVSphere::createSpherePoints(GLfloat radius, GLuint slices, GLuint stacks)
{
	// use iso convension:
	//    ¦È : polar angle
	//    ¦Õ : azimuthal angle
	std::vector<QVector3D>  points;
	points.reserve((stacks + 1) * (slices + 1));

	GLfloat thetaStep = M_PI / stacks;
	GLfloat phiStep = M_PI * 2 / slices;

	for (int i = 0; i <= stacks; ++i)
	{
		GLfloat theta = i * thetaStep;
		GLfloat sinTheta = std::sin(theta);
		GLfloat cosTheta = std::cos(theta);

		for (int j = 0; j <= slices; j++)
		{
			GLfloat phi = j == slices ? 0 : phiStep * j;
			points.push_back(
				radius * QVector3D(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta));
		}
	}

	return points;
}

