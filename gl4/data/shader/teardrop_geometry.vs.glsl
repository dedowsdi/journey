#version 430 core

layout(location = 0) in vec4 vertex;
layout(location = 0) uniform mat4 m_mat;

out vec3 w_vertex;

void main(void)
{
  w_vertex = (m_mat * vertex).xyz;
}
