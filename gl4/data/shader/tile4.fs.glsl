#version 430 core

uniform vec2 resolution;
uniform float time;

out vec4 frag_color;

float circle(vec2 st, float radius, float smoothedge2)
{
  vec2 ray = st - vec2(0.5);
  float radius2 = radius * radius;
  float ray_length2 = dot(ray,ray);
  return smoothstep(ray_length2, ray_length2+smoothedge2, radius2);
}

vec2 brick_tile(vec2 st, vec2 times)
{
  st *= times;
  // offset row in even time, column in odd time
  float mt = mod(time, 2.0);
  vec2 time_factor = vec2(step(mt, 1), step(1, mt));

  // offset even row or col to positive, odd row or row to negative
  vec2 row_col_factor = step(mod(st, 2), vec2(1)) * 2 - 1;
  row_col_factor.xy = row_col_factor.yx;

  st += time_factor * row_col_factor * -mod(time,1); 

  return fract(st);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st = brick_tile(st, vec2(20));

  vec3 bg_color = vec3(1);
  vec3 fg_color = vec3(0);

  vec3 color = mix(bg_color, fg_color, circle(st, 0.4, 0.05));

  frag_color = vec4(color, 1);
}
