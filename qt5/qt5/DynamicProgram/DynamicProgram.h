#pragma once

#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QDateTime>
#include <map>

class DynamicProgram : public QOpenGLShaderProgram
{
public:
	void addShaderFile(QOpenGLShader::ShaderType type, const QString& fileName);
	void removeShaderFile(QOpenGLShader::ShaderType type, const QString& fileName);

	// recompile if any file changed
	void update();

private:
	struct ShaderFile
	{
		QOpenGLShader* shader;
		QString file;
		QDateTime mtime;
	};

	std::multimap<QString, ShaderFile> _shaderFiles;
};

