#version 430 core

// _PP_PLACEHOLDER_

// _LIGHTING_PLACEHOLDER_

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;
#ifdef WITH_TEXTURE
layout(location = 2) in vec2 texcoord;
#endif

layout(location = 0) uniform mat4 mvp_mat;
layout(location = 1)uniform vec3 eye_pos;

out vertex_data
{
  vec4 color;

#ifndef SINGLE_COLOR
  vec4 specular_color;
#endif

#ifdef WITH_TEXTURE
  vec2 texcoord;
#endif

}vo;

void main(void)
{
  gl_Position = mvp_mat * vertex;

  vec3 vertex_ = vertex.xyz / vertex.w;

  separate_color colors =
    blinn_separate(vertex_, normal, eye_pos, lm, lights, mtl);

#ifdef SINGLE_COLOR
  vo.color = colors.diffuse + colors.specular;
  vo.color.a = colors.diffuse.a;
#else
  vo.color = colors.diffuse;
  vo.specular_color = colors.specular;
#endif

#ifdef WITH_TEXTURE
  vo.texcoord = texcoord;
#endif

}
