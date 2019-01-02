#version 430 core

#define PI 3.1415926535897932384626433832795

uniform vec2 resolution;
out vec4 frag_color;

float random(vec2 st)
{
  return fract(sin(dot(st, vec2(12.9898,78.233))) * 43758.5453123);
}

float random(float f)
{
  return fract(sin(f) * 10000);
}


float plot(vec2 st, float y, float smoothedge)
{
  float w = smoothedge / 2;
  return smoothstep(y - w, y, st.y) - smoothstep(y, y + w, st.y);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;

  vec3 bg_color = vec3(1);
  vec3 fg_color = vec3(0);

  st.x *= 5;
  float i = floor(st.x);
  float f = fract(st.x);

  float y = mix(random(i), random(i+1), smoothstep(0, 1, f));

  vec3 color = mix(bg_color, fg_color, plot(st, y, 0.02));

  frag_color = vec4(color, 1);
}
