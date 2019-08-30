#version 430 core

// _PP_PLACEHOLDER_

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;
#ifdef WITH_TEXTURE
layout(location = 2) in vec2 texcoord;
#endif

layout(location = 0) uniform mat4 mvp_mat;
layout(location = 1) uniform mat4 mv_mat;
layout(location = 2) uniform mat4 mv_mat_it;

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
  vo.vertex = (mv_mat * vertex/vertex.w).xyz;
  vo.normal = mat3(mv_mat_it) * normal;
#ifdef WITH_TEXTURE
  vo.texcoord = texcoord;
#endif
}
