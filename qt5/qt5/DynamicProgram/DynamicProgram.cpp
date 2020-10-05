#include "DynamicProgram.h"
#include <QFile>
#include <QFileInfo>

void DynamicProgram::addShaderFile(QOpenGLShader::ShaderType type, const QString& fileName)
{
	QOpenGLShader* shader = new QOpenGLShader(type, this);
	shader->compileSourceFile(fileName);
	addShader(shader);

	QFileInfo f(fileName);
	ShaderFile sf{ shader, fileName, f.lastModified() };
	_shaderFiles.insert(std::make_pair(fileName, sf));
}

void DynamicProgram::removeShaderFile(QOpenGLShader::ShaderType type, const QString& fileName)
{
}

void DynamicProgram::update()
{
	bool requireLink = false;
	for (auto& pair : _shaderFiles)
	{
		auto& sf = pair.second;
		QFileInfo f(sf.file);
		auto mtime = f.lastModified();
		if (mtime != sf.mtime)
		{
			sf.mtime = mtime;
			auto type = sf.shader->shaderType();

			removeShader(sf.shader);

			sf.shader = new QOpenGLShader(type, this);
			sf.shader->compileSourceFile(sf.file);

			addShader(sf.shader);

			requireLink = true;
		}
	}

	if (requireLink)
	{
		qDebug() << "Relink program " << this;
		link();
	}
}
