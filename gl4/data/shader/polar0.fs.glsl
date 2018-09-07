#version 430 core

uniform vec2 resolution;

out vec4 frag_color;

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  vec2 ray = st - vec2(0.5);
  float r = length(ray) * 2.5;
  float a = atan(ray.y, ray.x);

  //float f = cos(a*3); // r = cos(3theta)
  //float f = abs(cos(a*2.5)) * 0.5 + 0.3;
  //float f = abs(cos(a*12.)*sin(a*3.))*.8+.1;
  float f = smoothstep(-0.5,1., cos(a*10.0))*0.2+0.5;

  vec3 color = vec3(1 - smoothstep(f, f+0.01, r));
  frag_color = vec4(color, 1);
  
}
