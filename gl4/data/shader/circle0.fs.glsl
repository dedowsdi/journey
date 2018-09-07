#version 430 core

uniform vec2 resolution;
out vec4 frag_color;

float circle(vec2 st, float radius)
{
  vec2 ray = st - vec2(0.5);
  float radius2 = radius * radius;
  float ray_length2 = dot(ray,ray);
  return smoothstep(ray_length2, ray_length2+0.002, radius2);
}

float circle_ring(vec2 st, float inner_radius, float outer_radius)
{
  float t0 = circle(st, inner_radius);
  float t1 = circle(st, outer_radius);
  return t1 * (1-t0);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy/resolution;

  vec3 bg_color = vec3(0);
  vec3 fg_color = vec3(0, 0.5, 1);

  vec3 color = mix(bg_color, fg_color, circle(st, 0.15));
  color = mix(color, fg_color, circle_ring(st, 0.25, 0.3));

  frag_color = vec4(color, 1);
  
}
