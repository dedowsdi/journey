//#version 430 core

#ifndef MAX_ITERATIONS
#define MAX_ITERATIONS 500
#endif

#define BOUNDARY 64
#define BOUNDARY2 4096
#define ONE_OVER_LOG2 1.44269504088896f
#define ONE_OVER_LOG_BOUNDARY = 4.158883083359671

uniform dvec2 resolution;
uniform dvec4 rect; // left bottom right top

#ifdef DIRECT_DRAW

#ifndef NUM_COLORS
#define NUM_COLORS 256
#endif
uniform vec4 colors[NUM_COLORS];
out vec4 frag_color;

#else
out float frag_color;
#endif

// f(z) = z*z + c, z starts as 0, c is some complex number
// f(0) = c
// f(f(0)) = c*c + c
// ...
// it does not diverte(remains bounded in absolute value)
float mandelbrot_set(dvec2 st)
{
  // use st as complex plane(x as real, y as imaginary)
  dvec2 c = st;
  dvec2 z = dvec2(0);

  int i = 0;
  while (dot(z,z) < BOUNDARY2 && i < MAX_ITERATIONS) 
  {
    ++i;
    z = dvec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;
  }

  if(i < MAX_ITERATIONS)
  {
    //float log_zn = log( float(dot(z, z)) ) * 0.5f;
    //float nu = log(log_zn * ONE_OVER_LOG2) * ONE_OVER_LOG_BOUNDARY;
    //return float(i) + 1 - nu;
    return max(0.0f, float(i)  + 1 - log2(log2(float(dot(z, z))) * 0.5f));
  }
  else
  {
    return float(i);
  }
}

void main(void)
{
  dvec2 st = gl_FragCoord.xy / resolution;
  float iterations = float(mandelbrot_set(mix(rect.xy, rect.zw, st)));

#ifdef DIRECT_DRAW
  int c0 = int(floor(iterations*NUM_COLORS/MAX_ITERATIONS )) % NUM_COLORS;
  int c1 = (c0+1) % NUM_COLORS;
  frag_color = mix(colors[c0], colors[c1], fract(iterations));
#else
  frag_color = iterations;
#endif

}
