#version 430 core

#define PI 3.1415926535897932384626433832795

uniform vec2 resolution;
uniform float time;
out vec4 frag_color;

float random(vec2 st)
{
  return fract(sin(dot(st, vec2(12.9898,78.233))) * 43758.5453123);
}

float random(float f)
{
  return fract(sin(f) * 10000);
}

float rect(vec2 st, vec4 p, float smoothedge)
{
  vec2 t0 = smoothstep(p.xy, p.xy+smoothedge, st);
  vec2 t1 = 1-smoothstep(p.zw-smoothedge, p.zw, st);
  return t0.x * t0.y * t1.x * t1.y;
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st *= vec2(1, 2);
  float dir = mod(st.y, 2) < 1 ? 1 : -1;
  float t = time/2;
  float ct = ceil(t);

  float speed = random(vec2(ct + 1000)) * 0.5;
  float num_cells = random(vec2(ct)) * 20;

  vec3 bg_color = vec3(1);
  vec3 fg_color = vec3(0);
  vec3 color = bg_color;

  st.x += speed * dir * fract(t);
  st = fract(st);

  float x0 = random(vec2(ct)+ 1000) * 0.05;

  while (x0 < 1) 
  {
    float size = random(vec2(ct+x0*1000 + dir *1000)) * 0.05;
    color = mix(color, fg_color, rect(st, vec4(x0, 0, x0+size, 1), 0.001));
    x0 += size + random(vec2(ct+x0*1000 + dir * 1000)) * 0.1;
  }

  frag_color = vec4(color, 1);

}
