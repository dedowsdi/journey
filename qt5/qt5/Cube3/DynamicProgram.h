#pragma once

#include <QOpenGLShaderProgram>

class DynamicProgram : public QOpenGLShaderProgram
{
public:
	void addShaderFile(QOpenGLShader::ShaderType type, const QString& fileName);
	void removeShaderFile(QOpenGLShader::ShaderType type, const QString& fileName);

	// recompile if any file changed
	void update();

private:

};

