#version 120

attribute vec3 normal;
attribute vec2 texcoord;

varying vec3 m_normal;

void main(void)
{
  m_normal = normal;
  //inorder to render normal at resolution * tex, generate ndc from tex
  gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);
}
