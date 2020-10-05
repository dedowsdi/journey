#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>

class UVSphere
{
public:

	GLfloat radius() const { return _radius; }
	void setRadius(GLfloat v) { _radius = v; }

	GLuint slices() const { return _slices; }
	void setSlices(GLuint v) { _slices = v; }

	GLuint stacks() const { return _stacks; }
	void setStacks(GLuint v) { _stacks = v; }

	// If legacy is true (This sphere has only vertex, normal, texcoord 0):
	// 0 vertex
	// 2 normal
	// 3 colors
	// 4 second color
	// 5 fog color
	// 8-15 text coords
	// 
	// Otherwise:
	// 0 vertex
	// 1 normal
	// 2 colors
	// 3-10 text coords
	// 11 second color
	// 12 fog color
	bool legacy() const { return _legacy; }
	void setLegacy(bool b) { _legacy = b; }

	void draw();

	std::vector<QVector3D> createSpherePoints(GLfloat radius, GLuint slices, GLuint stacks);

private:
	void init();

	GLfloat _radius{ 1 };
	GLuint _slices{ 16 };  // longitiude
	GLuint _stacks{ 16 };  // latitude

	bool _support = true;
	bool _legacy = false;

	QOpenGLVertexArrayObject _vao;
	QOpenGLBuffer _vbo{ QOpenGLBuffer::VertexBuffer };
	QOpenGLBuffer _ibo{ QOpenGLBuffer::IndexBuffer };
};

