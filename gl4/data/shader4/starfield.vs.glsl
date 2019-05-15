#version 430 core

in vec4 vertex;

void main(void)
{
  gl_Position = vec4(vertex.xy / vertex.z, 1 - vertex.z, 1);
  gl_PointSize = 5.0f * 1.0f / vertex.z;
}
