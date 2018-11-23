//#version 330 core

layout(location = 0) in vec4 vertex;

#ifdef WITH_TEXCOORD
layout(location = 1) in vec2 texcoord;
out VS_OUT
{
  vec2 texcoord;
}vo;
#endif

#ifdef WITH_COLOR
in vec4 color;
out VS_OUT
{
  vec4 color;
}vo;
#endif

#ifdef INSTANCE
in mat4 m_mat;
#endif

uniform mat4 mvp_mat;

void main(void)
{
#ifdef WITH_TEXCOORD
  vo.texcoord = texcoord;
#endif
#ifdef WITH_COLOR
  vo.color = color;
#endif

#ifdef INSTANCE
  gl_Position = mvp_mat * m_mat * vertex;
#else
  gl_Position = mvp_mat * vertex;
#endif
}
