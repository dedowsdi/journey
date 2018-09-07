#version 430 core

#define PI 3.1415926

uniform vec2 resolution;
out vec4 frag_color;

float rect(vec2 st, vec4 p, float smoothedge)
{
  //vec2 t0 = step(p.xy, st);
  vec2 t0 = smoothstep(p.xy, p.xy+smoothedge, st);
  //vec2 t1 = 1-step(p.zw+smoothedge, st);
  vec2 t1 = 1-smoothstep(p.zw-smoothedge, p.zw, st);
  return t0.x * t0.y * t1.x * t1.y;
}

float rect_ring(vec2 st, vec4 inner, vec4 outer, float smoothedge)
{
  float t0 = rect(st, inner, smoothedge);
  float t1 = rect(st, outer, smoothedge);
  return t1 * (1-t0);
}

mat2 rotate2d(float a)
{
  float c = cos(a);
  float s = sin(a);
  return mat2(c,s,-s,c);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy/resolution;

  vec3 bg_color = vec3(0);
  vec3 rect_color = vec3(0, 0.5, 1);
  vec3 ring_color = vec3(1, 0, 0);

  vec3 color = vec3(0);

  color = mix(color, rect_color, rect(st, vec4(0.1, 0.1, 0.3, 0.3), 0.03));
  color = mix(color, ring_color, rect_ring(st, vec4(0.55, 0.55, 0.65, 0.65),
        vec4(0.5, 0.5, 0.7, 0.7), 0.01));

  frag_color = vec4(color, 1);
}
