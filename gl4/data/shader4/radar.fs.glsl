#version 430 core

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185

uniform vec2 resolution;
uniform float time;
out vec4 frag_color;

// st must between -1 and 1
float circle(vec2 st, float radius, float width)
{
  float r0 = radius - width/2;
  float r1 = radius + width/2;
  //r0 = r0*r0;
  //r1 = r1*r1;
  //float s = dot(st, st);
  float s = length(st);
  return smoothstep(r0, r0+width/2, s) * smoothstep(s, s+width/2, r1);
}

float circle(vec2 st, float radius)
{
  float l = length(st);
  return smoothstep(l, l+0.005, radius);
}

// special circle for target
float circle1(vec2 st, float radius, float width)
{
  float r0 = radius - width/2;
  float r1 = radius + width/2;
  //r0 = r0*r0;
  //r1 = r1*r1;
  //float s = dot(st, st);
  float s = length(st);
  return smoothstep(r0, r0+width/2, s) * smoothstep(s, s+0.01, r1);
}


float arc(vec2 st, float radius, float theta0, float theta1, float width)
{
  float r0 = radius - width/2;
  float r1 = radius + width/2;
  float t = atan(-st.y, -st.x) + PI;

  float s = length(st);
  return step(theta0, t) * step(t, theta1) * smoothstep(r0, radius, s) * smoothstep(s, s+width/2, r1);
}

float target(vec2 st)
{
  float a = circle(st, 0.02) * (sin(time*30)+1)*0.5;
  float b = circle(st, 0.03, 0.015);

  float r = mod(time, 0.9) * 0.2;
  float c = circle1(st, 0.08 + r, 0.08);
  return a + b + c;
}

mat2 rotate2d(float a)
{
  float c = cos(a);
  float s = sin(a);
  return mat2(c,s,-s,c);
}

float line(vec2 st, float r, float theta)
{
  float t = atan(-st.y, -st.x) + PI;

  float a = step(dot(st, st), r*r);
  float b = step(abs(t-theta), 0.001);
  return a*b;
}

// x positive line
float line1(vec2 st, float r, float width)
{
  float hw = width/2;
  return step(0, st.x) * step(st.x, r) * smoothstep(-hw, 0, st.y) *
    smoothstep(st.y, st.y + hw, hw);
}

float fan(vec2 st, float radius, float theta)
{
  float t = atan(-st.y, -st.x) + PI;
  //float t = atan(st.y, st.x);
  //return smoothstep(0, t, theta) * step(length(st), radius);
  float l = length(st);
  return smoothstep(-theta, 0, -t) * smoothstep(l, l+0.008, radius);
}

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

  vec2 st = gl_FragCoord.xy / resolution;
  vec2 cst = st * 2 - 1;

  vec3 bg_color = vec3(0);
  vec3 color = vec3(0);
  vec3 circle_color0 = vec3(0, 0, 1);
  vec3 circle_color1 = vec3(1, 1, 1);
  vec3 line_color0 = vec3(0.5);
  vec3 line_color1 = vec3(0, 0.5, 1);
  vec3 arc_color0 = vec3(0, 0.5, 0.5);
  vec3 arc_color1 = vec3(0, 0.75, 0.5);
  vec3 fan_color = vec3(0, 0.5, 1);
  vec3 triangle_color = vec3(1, 1, 1);
  vec3 target_color0 = vec3(1, 0, 0);
  vec3 target_color1 = vec3(1, 1, 1);

  color = mix(color, circle_color0, circle(cst, 0.03, 0.01));
  color = mix(color, circle_color0, circle(cst, 0.2, 0.01));
  color = mix(color, circle_color0, circle(cst, 0.35, 0.01));
  color = mix(color, circle_color1, circle(cst, 0.5, 0.015));

  color = mix(color, line_color0, line(cst, 0.5, PI*1/4));
  color = mix(color, line_color0, line(cst, 0.5, PI*3/4));
  color = mix(color, line_color0, line(cst, 0.5, PI*5/4));
  color = mix(color, line_color0, line(cst, 0.5, PI*7/4));

  float a = sin(time) * PI / 12;
  color = mix(color, arc_color0, arc(cst, 0.6, PI*3/16+a, PI*13/16-a, 0.01));
  color = mix(color, arc_color0, arc(cst, 0.6, PI*3/16+PI+a, PI*13/16+PI-a, 0.01));

  float gap = PI/32;
  float half_gap = gap/2;
  for (int i = 0; i < 8; ++i) 
    color = mix(color, arc_color1, arc(cst, 0.8, half_gap + i*PI/4, -half_gap + (i+1)*PI/4, 0.035));

  vec2 b = vec2(0, sin(time)*0.1);
  color = mix(color, triangle_color, polygon((cst+vec2(-0.7, 0))*rotate2d(PI*0.5) + b,  3, 0.01) );
  color = mix(color, triangle_color, polygon((cst + vec2(0, -0.7))*rotate2d(PI)+b, 3, 0.01) );
  color = mix(color, triangle_color, polygon((cst+vec2(0.7, 0))*rotate2d(-PI*0.5)+b, 3, 0.01) );
  color = mix(color, triangle_color, polygon((cst + vec2(0, 0.7)+b), 3, 0.01) );

  color = mix(color, target_color0, target(cst + vec2(0.25, 0)));
  color = mix(color, target_color1, circle(cst + vec2(-0.25, 0.25), 0.015));

  mat2 m = rotate2d(-time);
  color = mix(color, fan_color, fan(m*cst, 0.61, PI * 1/3) * 0.75);
  color = mix(color, line_color1, line1(m*cst, 0.61, 0.02));

  frag_color = vec4(color, 1);
}
