#version 430 core

in vs_out
{
 vec2 texcoord;
}fi;

const float PI = 3.1415926;

uniform sampler2D quad_map;
uniform float time;

uniform float amplitude = 0.2;
uniform float period = 0.25; // normalized period in s
uniform float wave_period = 1.5; // only used on mode 1, peroid of one wave

/*
 * 0 : total sine wave
 * 1 : wave forward
 */
uniform int mode = 0;

out vec4 frag_color;

vec2 mode0(vec2 st)
{
  st.t -= sin(st.s * PI * 2 / period - time) * amplitude;
  return st;
}

vec2 mode1(vec2 st)
{
  // get start s position of current wave, a wave's start position belongs to
  // [-peroid, 1]
  float l = mod(time, wave_period) / wave_period * (1 + period) - period;
  if(st.s >= l && st.s <= l + period)
  {
    st.t -= sin( (st.s - l) * PI * 2 / period) * amplitude;
  }
  return st;
}

void main(void)
{
  vec2 st;
  switch(mode)
  {
    case 0:
      st = mode0(fi.texcoord);
      break;
    
    case 1:
      st = mode1(fi.texcoord);
      break;
  }

  frag_color = texture(quad_map, st);
}
