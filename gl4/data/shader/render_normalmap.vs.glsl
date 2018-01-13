#version 430 core

in vec3 normal;
in vec2 texcoord;

out VS_OUT{
  vec3 normal;
}vs_out;

void main(void)
{
  vs_out.normal = normal;
  //inorder to render normal at resolution * tex, generate ndc from tex
  gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);
}
