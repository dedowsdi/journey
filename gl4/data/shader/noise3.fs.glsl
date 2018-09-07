#version 430 core

#define PI 3.1415926535897932384626433832795

uniform vec2 resolution;
out vec4 frag_color;

float random(vec2 st)
{
  return fract(sin(dot(st, vec2(12.9898,78.233))) * 43758.5453123);
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

mat2 rotate2d(float a)
{
  float c = cos(a);
  float s = sin(a);
  return mat2(c,s,-s,c);
}


float lines(in vec2 pos, float b){
  float scale = 10.0;
  pos *= scale;
  return smoothstep(0.0, .5 + b*.5,
                  abs((sin(pos.y*3.1415)+b*2.0))*.5);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy/resolution.xy;
  st.y *= resolution.y/resolution.x;

  vec2 pos = st.xy*vec2(3.,10.);

  float pattern = pos.y;

  // Add noise
  pos = rotate2d( -noise(pos) ) * pos;

  // Draw lines
  pattern = lines(pos,.5);

  frag_color = vec4(vec3(pattern),1.0); 
}

