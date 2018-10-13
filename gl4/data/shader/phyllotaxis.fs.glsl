#version 330 core
#define PI 3.1415926535897932384626433832795
#define NUM_POINTS 1000
// 137.5 is the "right value", you can try any angle you want
#define THETA 137.5

uniform vec2 resolution;

out vec4 frag_color;

float circle(vec2 st, float radius)
{
  float radius2 = radius * radius;
  float smooth_edge = radius * 0.5;
  smooth_edge *= smooth_edge;
  return 1 - smoothstep(radius2-smooth_edge, radius2, dot(st, st));
  //return step(dot(st, st), radius2);
}


void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st = st*2 - 1;

  vec3 bg_color = vec3(0);
  vec3 fg_color = vec3(1);
  vec3 color = vec3(0);

  float angle = THETA/180.0 * PI;
  float max_r = sqrt(NUM_POINTS) * 1.05;
  float c = 1 / max_r;
  for (int i = 0; i < NUM_POINTS; i++) {
    float theta = angle * i;
    float r = sqrt(i) * c;
    vec2 pos = vec2(r*cos(theta), r*sin(theta));

    vec2 st1 = st - pos;
    color += mix(bg_color, fg_color, circle(st1, 0.025));
  }

  frag_color = vec4(color, 1);
}
