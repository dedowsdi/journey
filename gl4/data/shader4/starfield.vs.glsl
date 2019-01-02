#version 430 core

in vec4 vertex;

void main(void)
{
  gl_Position = vec4(vertex.xy * vertex.z, 0, 1);
  gl_PointSize = vertex.z * 3000.0;
  //gl_PointSize = 30;
}
