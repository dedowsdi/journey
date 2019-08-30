#version 430 core

// _PP_PLACEHOLDER_

// _LIGHTING_PLACEHOLDER_

in vertex_data
{
  vec3 vertex;
  vec3 normal;
#ifdef WITH_TEXTURE
  vec2 texcoord;
#endif
}fi;

#ifdef WITH_TEXTURE
layout(location = 3) uniform sampler2D diffuse_map;
#endif

out vec4 frag_color;

void main(void)
{
  separate_color colors =
    blinn_separate(fi.vertex, fi.normal, vec3(0), lm, lights, mtl);

  frag_color = colors.diffuse;

#ifdef SINGLE_COLOR
  frag_color.xyz += colors.specular.xyz;

#ifdef WITH_TEXTURE
  frag_color.xyz *= texture(diffuse_map, fi.texcoord).xyz;
#endif // WITH_TEXTURE

#else

#ifdef WITH_TEXTURE
  frag_color.xyz *= texture(diffuse_map, fi.texcoord).xyz;
#endif

  frag_color.xyz += colors.specular.xyz;

#endif // SINGLE_COLOR

}
