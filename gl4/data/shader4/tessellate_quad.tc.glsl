#version 430 core

layout(vertices = 4) out;

void main()
{
  gl_TessLevelOuter[0] = 2.0f;
  gl_TessLevelOuter[1] = 2.0f;
  gl_TessLevelOuter[2] = 2.0f;
  gl_TessLevelOuter[3] = 2.0f;
  gl_TessLevelInner[0] = 2.0f;
  gl_TessLevelInner[1] = 2.0f;
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
