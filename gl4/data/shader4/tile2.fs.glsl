#version 430 core

#define PI 3.1415926535897932384626433832795

uniform vec2 resolution;
out vec4 frag_color;

float plot(vec2 st, float y, float smoothedge)
{
  float w = smoothedge / 2;
  return smoothstep(y - w, y, st.y) - smoothstep(y, y + w, st.y);
}

float plot_x(vec2 st, float x, float smoothedge)
{
  float w = smoothedge / 2;
  return smoothstep(x - w, x, st.x) - smoothstep(x, x + w, st.x);
}

float rect_rake_line(vec2 st, vec2 size, vec2 rake_size, float smoothedge)
{
  // create x, y, center symmetry
  st = abs((st-vec2(0.5))*2);
  // draw line in first quardant
  float t0 = plot(st, size.y, smoothedge) * step(st.x, size.x - rake_size.x);
  float t1 = plot_x(st, size.x, smoothedge) * step(st.y, size.y - rake_size.y);

  float a = -rake_size.y/rake_size.x * st.x + 1 + rake_size.y * (1 -
      rake_size.x)/rake_size.x;
  
  // border line will be 1.5 times wider than rake line (0.5 + 0.5(overlay) + 0.5)
  float t2 = plot(st, a, smoothedge*1.5);

  return t0 + t1 + t2;
}

vec2 rect_test(vec2 st)
{
  return st * (1 - st);
}

float rect_rake(vec2 st, vec2 size, vec2 rake_size, float smoothedge)
{
  vec2 t0 = rect_test(st);
  vec2 t1 = rect_test((1-size)/2);
  vec2 uv = smoothstep(t1, t1+smoothedge, t0);

  // create x, y, center symmetry
  st = abs((st-vec2(0.5))*2);

  // exclude graph above rake line
  float t = -rake_size.y/rake_size.x * st.x + 1 + rake_size.y * (1 -
      rake_size.x)/rake_size.x;
  float r = smoothstep(st.y, st.y + smoothedge, t);
  return uv.x * uv.y * r ;
}

float rect_rake_ring(vec2 st, vec2 size0, vec2 rake_size0, vec2 size1, vec2
    rake_size1, float smoothedge)
{
  float t0 = rect_rake(st, size0, rake_size0, smoothedge);
  float t1 = rect_rake(st, size1, rake_size1, smoothedge);
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

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;

  st = tile(st, 5, 5);

  vec3 graph_color = vec3(0.0);
  vec3 bg_color = vec3(1.0);

  // 0.707 = 0.5 * sqrt(2)
  //vec3 color = mix(bg_color, graph_color, rect_rake(st, vec2(0.98), vec2(0.25), 0.01));
  vec3 color = mix(bg_color, graph_color, rect_rake_line(st, vec2(1), vec2(0.25), 0.1));

  frag_color = vec4(color, 1);
}
