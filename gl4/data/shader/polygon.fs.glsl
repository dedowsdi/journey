#version 430 core

#define PI 3.14159265359
#define TWO_PI 6.28318530718

uniform vec2 resolution;

// st must range from -1 to 1
float polygon(vec2 st, int numSides, float size)
{
  // Angle and radius from the current pixel
  //float a = atan(st.y,st.x);
  float a = atan(st.x,st.y) + PI;
  float r = TWO_PI/float(numSides);

  // Shaping function that modulate the distance
  float d = cos(floor(.5+a/r)*r-a)*length(st);
  return 1 - smoothstep(size, size+0.01, d);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy/resolution.xy;
  st.x *= resolution.x/resolution.y;

  // Remap the space to -1. to 1.
  st = st *2.-1.;
  vec3 color = vec3(polygon(st, 3, 0.4));

  gl_FragColor = vec4(color,1.0);
}
