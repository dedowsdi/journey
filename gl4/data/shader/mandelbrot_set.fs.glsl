#version 430 core

#define NUM_ITERATIONS 100
#define BOUNDARY 8

uniform vec2 resolution;

out vec4 frag_color;

// f(z) = z*z + c, z starts as 0, c is some complex number
// f(0) = c
// f(f(0)) = c*c + c
// ...
// it does not diverte(remains bounded in absolute value)
float mandelbrot_set(vec2 st)
{
  // use st as complex plane(x as real, y as imaginary)
  float ca = st.x;
  float cb = st.y;

  // firt iterations
  float za = st.x;
  float zb = st.y;

  // NUM_ITERATOIONS - 1 iterations
  int i = 0;
  while (++i < NUM_ITERATIONS) 
  {
    float a = za*za - zb*zb;
    float b = 2 * za * zb;
    za = a + ca;
    zb = b + cb;

    if(abs(za + zb) > BOUNDARY)
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
  st *= 1.5;
  st -= vec2(0.5,0);

  vec3 bg_color = vec3(0, 0.023529, 0.360784);
  vec3 fg_color = vec3(1);

  float pct = mandelbrot_set(st);
  vec3 color = mix(bg_color, fg_color, pct);
  if(pct == 0)
    color = vec3(0);

  frag_color = vec4(color, 1);
  
}
