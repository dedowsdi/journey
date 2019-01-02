//#version 430 core

#ifndef NUM_COLORS
#define NUM_COLORS 256
#endif

in vs_out
{
 vec2 texcoord;
}fi;

uniform sampler2D iteration_map;
uniform samplerBuffer hue_buffer;
uniform float itotal;
out vec4 frag_color;

uniform vec4 colors[NUM_COLORS];

void main(void)
{
  float iterations = texture(iteration_map, fi.texcoord).r;
  float hue0 = texelFetch(hue_buffer, int(iterations)).r ;
  float hue1 = texelFetch(hue_buffer, int(ceil(iterations))).r ;
  float hue = mix(hue0, hue1, fract(iterations)) * itotal;
  //hue *= hue;
  int c0 = int(floor(hue * (NUM_COLORS - 1)));
  frag_color = colors[c0];
  //frag_color = vec4(hue);
}
