#version 330 core

in vec4 vertex; // x y s t

out VS_OUT{
  vec2 texcoord;
}vs_out;

uniform mat4 modelViewProjMatrix;

void main() {
  vs_out.texcoord = vertex.yz;
  gl_Position = modelViewProjMatrix * vec4(vertex.xy, 0, 1);
}
