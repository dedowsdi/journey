attribute vec4 vertex;

uniform mat4 modelViewProjMatrix;

void main(void)
{
  gl_Position = modelViewProjMatrix * vertex;
}
