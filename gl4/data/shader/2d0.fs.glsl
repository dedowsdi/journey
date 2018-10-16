//#version 330 core

#ifdef WITH_TEXCOORD
in VS_OUT
{
  vec2 texcoord;
}fi;
uniform sampler2D diffuse_map;
#else
uniform vec4 color;
#endif

out vec4 frag_color;

void main(void)
{
#ifdef WITH_TEXCOORD
  frag_color = texture(diffuse_map, fi.texcoord);
#else
  frag_color = color;
#endif
}
