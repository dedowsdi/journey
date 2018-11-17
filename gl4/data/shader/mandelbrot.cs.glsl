//#version 430 core

layout (local_size_x = 16, local_size_y = 16) in;

layout (r32f, binding = 0)  uniform image2D img;
layout (location = 0) uniform dvec4 rect; // left bottom right top

#ifndef MAX_ITERATIONS
#define MAX_ITERATIONS 500
#endif

#define BOUNDARY 256
#define BOUNDARY2 65536
#define ONE_OVER_LOG2 1.44269504088896f

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
  dvec2 c = mix(rect.xy, rect.zw,
          dvec2(gl_GlobalInvocationID.xy + dvec2(0.5))/(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy));
  float iterations = float(mandelbrot_set(c));
  imageStore(img, ivec2(gl_GlobalInvocationID.xy), vec4(iterations));
}
