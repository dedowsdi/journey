#version 430 core

in vs_out
{
  vec2 texcoord;
}fi;

uniform usampler2D diffuse_map;
uniform vec4 color0;
uniform vec4 color1;
uniform vec4 color2;
uniform vec4 color3;
uniform vec4 color4;

out vec4 frag_color;

void main(void)
{
  uint r = texture(diffuse_map, fi.texcoord).r;
  //frag_color = vec4(float(r)/7.0);
  switch(r)
  {
    case 0:
      frag_color = color0;
      break;

    case 1:
      frag_color = color1;
      break;

    case 2:
      frag_color = color2;
      break;

    case 3:
      frag_color = color3;
      break;

    default:
      frag_color = color4;
  }
}
