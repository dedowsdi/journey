#version 430 core

uniform vec2 resolution;
out vec4 frag_color;

float circle(in vec2 st, in float _radius){
    vec2 l = st-vec2(0.5);
    return 1.-smoothstep(_radius-(_radius*0.01),
                         _radius+(_radius*0.01),
                         dot(l,l)*4.0);
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
  st = tile(st, 3, 3);
  frag_color = vec4(circle(st, 0.5));
}
