#pragma once

#include <vector>
#include <QOpenGLWidget>
#include <QVector2D>

// mesh util
namespace mutil
{

// You have stacks+1 rows of vertices, each row has slices+1 cols, this function
// create triangle strip elements as adbecf... or daebfc... if reversed:
//  a b c ...
//  d e f ...
//
// This function also works if your grid is created slice by slice, it results in adbecf...
// for following vertices.
//  . .
//  c f
//  b e
//  a d
// require support for GL_PRIMITIVE_RESTART_FIXED_INDEX
std::vector<GLuint> createGridIndices(int stacks, int slices, int offset = 0, bool reverse = false);


// texcoord0 : texcoord of 1st vertex in 1st stack
// texcoord1 : texcoord of last vertex in last stack
std::vector<QVector2D> createGridTexcoordsStackByStack(int stacks, int slices,
	const QVector2D& texcoord0 = QVector2D(0, 1),
	const QVector2D& texcoord1 = QVector2D(1, 0));

std::vector<QVector2D> createGridTexcoordsSliceBySlice(int slices, int stacks,
	const QVector2D& texcoord0 = QVector2D(0, 0),
	const QVector2D& texcoord1 = QVector2D(1, 1));
};

