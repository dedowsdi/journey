#version 330 core

in vs_out
{
 vec2 texcoord;
}fi;

uniform float exposure = 1;
uniform sampler2D hdr_map;
uniform sampler2D brightness_map;

out vec4 frag_color;

void main(void)
{
  vec4 color = texture(hdr_map, fi.texcoord) +
    texture2D(brightness_map, fi.texcoord);
  
  frag_color = vec4(1.0) - exp(-color * exposure);
}
