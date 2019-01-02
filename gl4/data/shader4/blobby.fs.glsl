#version 330 core

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

float circle(vec2 st, float radius, float smoothedge)
{
  float end = radius * radius;
  float start = end + smoothedge * smoothedge - 2 * radius * smoothedge;
  return 1 - smoothstep(start, end, dot(st, st));
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st = st*2 - 1;

  vec3 bg_color = vec3(0);
  vec3 fg_color = vec3(1);

  float angle = atan(st.y, st.x);
  //float radius = 0.8 + sin(angle * 7 + time)*0.15;
  float radius = 0.8 + noise(st + vec2(time)) * 0.15;

  vec3 color = mix(bg_color, fg_color, circle(st, radius, 0.02));

  frag_color = vec4(color, 1);
}

