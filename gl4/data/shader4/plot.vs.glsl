#version 430 core

void main(void)
{
  vec4 vertices[4] = {
    {-1, 1 , 0, 1},
    {-1, -1, 0, 1},
    {1 , 1 , 0, 1},
    {1 , -1, 0, 1}
  };
  gl_Position = vertices[gl_VertexID];
}
