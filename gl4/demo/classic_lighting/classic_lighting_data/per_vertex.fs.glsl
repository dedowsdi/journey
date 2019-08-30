#version 430 core

// _PP_PLACEHOLDER_

#ifdef WITH_TEXTURE
layout(location = 3) uniform sampler2D diffuse_map;
#endif

in vertex_data
{
  vec4 color;

#ifndef SINGLE_COLOR
  vec4 specular_color;
#endif

#ifdef WITH_TEXTURE
  vec2 texcoord;
#endif

}fi;

out vec4 frag_color;

void main(void)
{

  frag_color = fi.color;

#ifdef WITH_TEXTURE
  frag_color.xyz *= texture(diffuse_map, fi.texcoord).xyz;
#endif

#ifndef SINGLE_COLOR
  frag_color.xyz += fi.specular_color.xyz;
#endif

}
