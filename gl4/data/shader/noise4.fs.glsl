#version 430 core

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185

uniform vec2 resolution;
uniform float time;
out vec4 frag_color;

float random(vec2 st)
{
  return fract(sin(dot(st, vec2(12.9898,78.233))) * 43758.5453123);
}

float random(float f)
{
  return fract(sin(f) * 10000);
}

// Value noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/lsf3WH
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( random( i + vec2(0.0,0.0) ),
                     random( i + vec2(1.0,0.0) ), u.x),
                mix( random( i + vec2(0.0,1.0) ),
                     random( i + vec2(1.0,1.0) ), u.x), u.y);
}

float circle_noise(vec2 st, float radius, float smoothedge)
{
  st = st*2 - vec2(1);
  float r = length(st);
  float theta = atan(st.y, st.x) ;
  // m must be continues when theta belongs to [-pi, pi], it should has only 1
  // peak no matter what time is!.
  float m = abs(mod(time*4+theta, TWO_PI) - PI) / PI;
  radius += sin(theta*50) * 0.05 + sin(theta*20) * pow(m, 2) * 0.1;

  return 1-smoothstep(radius-smoothedge, radius, r);
}

float circle_ring(vec2 st, float inner, float outer, float smoothedge)
{
  float t0 = circle_noise(st, inner, smoothedge);
  float t1 = circle_noise(st, outer, smoothedge);
  return t1 * (1-t0);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;

  vec3 bg_color = vec3(1);
  vec3 fg_color = vec3(0);

  //vec3 color = mix(bg_color, fg_color, circle_noise(st, 0.6, 0.01));
  vec3 color = mix(bg_color, fg_color, circle_ring(st, 0.57, 0.6, 0.015));

  frag_color = vec4(color, 1);
}
