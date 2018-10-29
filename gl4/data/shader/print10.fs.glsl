#version 330 core

// resolutin / scale must be int
#define SCALE vec2(16)
#define RAND_SEED 192

uniform vec2 resolution;
out vec4 frag_color;

float random(float f)
{
  return fract(sin(f) * 10000);
}

float plot(vec2 st, float y, float smoothedge)
{
  return smoothstep(y - smoothedge, y, st.y) 
    * (1 - smoothstep(y, y+smoothedge, st.y));
}


void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st *= SCALE;

  // create random forward or backword slash
  vec2 ist = floor(st);
  float r = step(0.5, random(RAND_SEED + ist.y * SCALE.x + ist.x));
  float multiplier = mix(-1, 1, r);
  float addend = mix(1, 0, r);
  st = fract(st);
  float y = st.x * multiplier + addend;

  vec3 bg_color = vec3(0);
  vec3 fg_color = vec3(1);


  vec3 color = mix(bg_color, fg_color, plot(st, y, 0.01));
  frag_color = vec4(color, 1);
}
