#version 430 core

in vs_out
{
 vec2 texcoord;
}fi;

uniform sampler2D diffuse_map;
uniform float time;

out vec4 frag_color;

float random(float f)
{
  return fract(sin(f) * 10000);
}

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

void main(void)
{
  vec2 texcoord = fi.texcoord + mix(-0.015, 0.015, noise(fi.texcoord*20 +
        vec2(time*50, 0)));

  frag_color = texture(diffuse_map, texcoord);
}


