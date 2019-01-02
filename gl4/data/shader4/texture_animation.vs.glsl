#version 330 core

in vec4 vertex;
in vec2 texcoord;

uniform mat4 mvp_mat;
uniform mat4 tex_mat;

out vs_out
{
  vec2 texcoord;
}vo;

void main(void)
{
  gl_Position = mvp_mat * vertex;
  vec4 t = tex_mat * vec4(texcoord, 0,1);
  vo.texcoord = t.xy/t.w;
}
