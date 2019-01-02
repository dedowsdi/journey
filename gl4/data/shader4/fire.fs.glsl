#version 430 core

in vs_out
{
 vec2 texcoord;
}fi;

uniform sampler2D fire_map;
uniform sampler2D cool_map;
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
  vec2 ts = 1.0 / textureSize(fire_map, 0);

  // smooth spread fire
  vec4 color = texture(fire_map, fi.texcoord + vec2(ts.x, -ts.y)) + 
               texture(fire_map, fi.texcoord + vec2(-ts.x, -ts.y)) + 
               texture(fire_map, fi.texcoord + vec2(0, 0)) + 
               texture(fire_map, fi.texcoord + vec2(0, -ts.y * 2));
  color *= 0.25;

  float cool = noise(fi.texcoord*25 + vec2(0, time*1.5)) * 0.02;
  color -= vec4(cool);

  //color -= texture(cool_map, fi.texcoord + vec2(0, -ts.y));

  frag_color = color;
}
