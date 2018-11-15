#version 430 core

#define NUM_ITERATIONS 100
#define BOUNDARY 2

uniform vec2 resolution;
uniform vec2 mouse;

out vec4 frag_color;

// the only difference between mandelbrot set and julia set is that julia set
// use fixed c.
// f(z) = z*z + c, z starts as 0, c is some fixed complex number
// f(0) = c
// f(f(0)) = c*c + c
// ...
// it does not diverte(remains bounded in absolute value)
float julia_set(vec2 st, vec2 c)
{
  // firt iterations
  float za = st.x;
  float zb = st.y;

  // NUM_ITERATOIONS - 1 iterations
  int i = 0;
  while (++i < NUM_ITERATIONS) 
  {
    float a = za*za - zb*zb;
    float b = 2 * za * zb;
    za = a + c.x;
    zb = b + c.y;

    if(abs(za) > BOUNDARY)
      break;
  }

  if(i == NUM_ITERATIONS)
    i = 0;

  return float(i) / NUM_ITERATIONS;
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st = st*2 - 1;
  st *= 2;
  vec2 c = mouse / resolution;
  c = c*2 - 1;

  vec3 bg_color = vec3(0, 0.023529, 0.360784);
  vec3 fg_color = vec3(1);

  float pct = julia_set(st, c);
  vec3 color = mix(bg_color, fg_color, pct);
  if(pct == 0)
    color = vec3(0);

  frag_color = vec4(color, 1);
  
}
