#version 430 core

in vec4 vertex;

uniform mat4 modelViewProjMatrix;

out VS_OUT{
  vec2 texcoord;
} vs_out;

void main(void)
{
  gl_Position = modelViewProjMatrix * vec4(vertex.xy, 0, 1);
  vs_out.texcoord = vertex.zw;
}
