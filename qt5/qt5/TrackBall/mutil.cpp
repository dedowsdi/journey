#include "mutil.h"

#include <QtMath>

namespace mutil
{
std::vector<GLuint> mutil::createGridIndices(int stacks, int slices, int offset, bool reverse)
{
	std::vector<GLuint> indices;
	indices.reserve(2 * (slices + 1));

	for (auto i = 0; i < stacks; ++i)
	{
		auto stack0 = (slices + 1) * i + offset;
		auto stack1 = stack0 + slices + 1;
		for (auto j = 0; j <= slices; j++)
		{
			if (reverse)
			{
				indices.push_back(stack1 + j);
				indices.push_back(stack0 + j);
			}
			else
			{
				indices.push_back(stack0 + j);
				indices.push_back(stack1 + j);
			}
		}
		indices.push_back(-1);
	}

	return indices;
}


namespace
{

inline GLfloat mix(GLfloat a, GLfloat b, GLfloat p)
{
	return a * (1 - p) + b * p;
}

}

std::vector<QVector2D> createGridTexcoordsStackByStack(int stacks, int slices, const QVector2D& texcoord0, const QVector2D& texcoord1)
{
	std::vector<QVector2D> texcoords;
	texcoords.reserve((stacks + 1) * (slices + 1));

	auto sStep = 1.0f / slices;
	auto tStep = 1.0f / stacks;
	for (auto i = 0; i <= stacks; ++i)
	{
		auto t = mix(texcoord0.y(), texcoord1.y(), i * tStep);
		for (auto j = 0u; j <= slices; j++)
		{
			auto s = mix(texcoord0.x(), texcoord1.x(), j * sStep);
			texcoords.push_back(QVector2D(s, t));
		}
	}

	return texcoords;
}

std::vector<QVector2D> createGridTexcoordsSliceBySlice(int slices, int stacks, const QVector2D& texcoord0, const QVector2D& texcoord1)
{
	std::vector<QVector2D> texcoords;
	texcoords.reserve((stacks + 1) * (slices + 1));

	auto sStep = 1.0f / slices;
	auto tStep = 1.0f / stacks;
	for (auto i = 0; i <= slices; ++i)
	{
		auto t = mix(texcoord0.y(), texcoord1.y(), i * tStep);
		for (auto j = 0u; j <= stacks; j++)
		{
			auto s = mix(texcoord0.x(), texcoord1.x(), j * sStep);
			texcoords.push_back(QVector2D(s, t));
		}
	}

	return texcoords;
}
}
