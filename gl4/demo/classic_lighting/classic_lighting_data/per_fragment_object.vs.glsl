#version 430 core

// _PP_PLACEHOLDER_

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;
#ifdef WITH_TEXTURE
layout(location = 2) in vec2 texcoord;
#endif

layout(location = 0) uniform mat4 mvp_mat;

out vertex_data
{
  vec3 vertex;
  vec3 normal;
#ifdef WITH_TEXTURE
  vec2 texcoord;
#endif
}vo;

void main(void)
{
  gl_Position = mvp_mat * vertex;
  vo.vertex = vertex.xyz / vertex.w;
  vo.normal = normal;
#ifdef WITH_TEXTURE
  vo.texcoord = texcoord;
#endif
}
