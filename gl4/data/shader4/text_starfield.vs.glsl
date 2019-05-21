#version 430 core

in layout(location = 0) vec4 vertex;
in layout(location = 1) vec2 texcoord;

uniform mat4 vp_mat;
uniform mat4 m_mat;

out vs_out
{
  vec2 texcoord;
}vo;

void main(void)
{
  mat4 m = m_mat;
  float z = m_mat[3][2];
  m[3][0] /= pow(-z/100, 1.5);
  m[3][1] /= pow(-z/100, 1.5);
  gl_Position = vp_mat * m * vertex;
  // translate in window space, note this also cause distortion
  vo.texcoord = texcoord;
}
