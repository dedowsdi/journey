#version 430 core

uniform vec2 resolution;
out vec4 frag_color;

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  vec2 ray = (st - vec2(0.5)) * 2;
  vec2 r = abs(ray);

  float m = max(r.x, r.y);
  //float s  = step(m, 0.5);
  //float s  = step(0.5, m) * step(m, 0.6);
  //float s  = smoothstep(0.5, 0.52, m) * smoothstep(m, m+0.02, 0.6);
  //vec2 l = max(r-0.3, 0);
  //float l2 = dot(l, l);
  //float s  = 1 - smoothstep(0.0398, 0.04, l2);
  float a=atan(ray.y,ray.x);
  // a*.636    scale to [-2,2]
  // ..+0.5    scale to [-1.5, 2.5]
  // floor     make step to [-2, 2]
  // .. * 1.57 scale to [-3.14, 3.14]
  frag_color = vec4(step(0.5, cos(floor(a*.636 + 0.5) * 1.57 - a) *
        length(ray.xy)));
  //frag_color = vec4(step(.5,cos( floor(a*.636+.5)* 1.57-a)*length(ray.xy)));

  //frag_color = vec4(s, s, s, 1);
}
