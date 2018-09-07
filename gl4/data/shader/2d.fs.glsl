#version 430 core

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185

uniform vec2 resolution;
uniform float time;
out vec4 frag_color;

vec2 rect_test(vec2 st)
{
  return st * (1 - st);
}

// create rect in screen center.
float rect(vec2 st, vec2 size, float smoothedge)
{
  vec2 t0 = rect_test(st);
  vec2 t1 = rect_test((1-size)/2);
  //vec2 uv = step(t0, t1);
  vec2 uv = smoothstep(t1, t1+smoothedge, t0);
  return uv.x * uv.y;
}

float rect_ring(vec2 st, vec2 inner_size, vec2 outer_size, float smoothedge)
{
  float t0 = rect(st, inner_size, smoothedge);
  float t1 = rect(st, outer_size, smoothedge);
  return t1 * (1-t0);
}

// create rect in random position and size
float rect(vec2 st, vec4 p, float smoothedge)
{
  vec2 t0 = smoothstep(p.xy, p.xy+smoothedge, st);
  vec2 t1 = 1-smoothstep(p.zw-smoothedge, p.zw, st);
  return t0.x * t0.y * t1.x * t1.y;
}

float rect_ring(vec2 st, vec4 inner, vec4 outer, float smoothedge)
{
  float t0 = rect(st, inner, smoothedge);
  float t1 = rect(st, outer, smoothedge);
  return t1 * (1-t0);
}

float circle(vec2 st, float radius, float smoothedge2)
{
  vec2 ray = st - vec2(0.5);
  float radius2 = radius * radius;
  float ray_length2 = dot(ray,ray);
  return smoothstep(ray_length2, ray_length2+smoothedge2, radius2);
}

float circle_ring(vec2 st, float inner_radius, float outer_radius, float smoothedge2)
{
  float t0 = circle(st, inner_radius, smoothedge2);
  float t1 = circle(st, outer_radius, smoothedge2);
  return t1 * (1-t0);
}

float polygon(vec2 cst, int num_sides, float size, float smoothedge)
{
  // Angle and radius from the current pixel
  float a = atan(cst.x,cst.y) + PI;
  float r = TWO_PI/float(num_sides);

  // Shaping function that modulate the distance
  float d = cos(floor(.5+a/r)*r-a)*length(cst);
  return 1 - smoothstep(size, size+smoothedge, d);
}

float fan(vec2 cst, float radius, float theta, float smoothedge)
{
  float t = atan(-cst.y, -cst.x) + PI;
  float l = length(cst);
  return step(t, theta) * smoothstep(l, l+smoothedge, radius);
}

float fan_ring(vec2 cst, float inner_radius, float outer_radius, float theta, float smoothedge)
{
  float t0 = fan(cst, inner_radius, theta, smoothedge);
  float t1 = fan(cst, outer_radius, theta, smoothedge);
  return t1 * (1-t0);
}

mat2 rotate2d(float a)
{
  float c = cos(a);
  float s = sin(a);
  return mat2(c,s,-s,c);
}

vec2 tile(vec2 st, int x, int y)
{
  st.x *= x;
  st.y *= y;
  return fract(st);
}

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

void main(void)
{
  
  
}

