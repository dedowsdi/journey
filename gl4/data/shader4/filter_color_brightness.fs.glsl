#version 330 core

in vs_out
{
 vec2 texcoord;
}fi;

uniform sampler2D quad_map;
uniform float threshold = 0.95;

out vec4 frag_color0;
out vec4 frag_color1;

void main(void) {
  vec4 color = texture(quad_map, fi.texcoord);
  // color
  frag_color0 = color;
  float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

  // brightness
  if (brightness >= threshold) {
    frag_color1 = vec4(color.xyz, 1);
  } else {
    frag_color1 = vec4(0, 0, 0, 1);
  }
}
