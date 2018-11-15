#version 430 core

#define MAX_ITERATIONS 800
#define BOUNDARY 1024
#define BOUNDARY2 1048576
//#define BOUNDARY 2
//#define BOUNDARY2 4
#define NUM_COLORS 64

uniform vec2 resolution;
uniform dvec2 start = dvec2(-0.580123, 0.488702) - dvec2(0.0025354);
uniform dvec2 end = dvec2(-0.580123, 0.488702) + dvec2(0.0025354);
//uniform dvec2 start = dvec2(-2);
//uniform dvec2 end = dvec2(2);

vec3 colors[] = {
  vec3( 66/255.0,  30/255.0,  15/255.0),
  vec3( 25/255.0,   7/255.0,  26/255.0),
  vec3(  9/255.0,   1/255.0,  47/255.0),
  vec3(  4/255.0,   4/255.0,  73/255.0),
  vec3(  0/255.0,   7/255.0, 100/255.0),
  vec3( 12/255.0,  44/255.0, 138/255.0),
  vec3( 24/255.0,  82/255.0, 177/255.0),
  vec3( 57/255.0, 125/255.0, 209/255.0),
  vec3(134/255.0, 181/255.0, 229/255.0),
  vec3(211/255.0, 236/255.0, 248/255.0),
  vec3(241/255.0, 233/255.0, 191/255.0),
  vec3(248/255.0, 201/255.0,  95/255.0),
  vec3(255/255.0, 170/255.0,   0/255.0),
  vec3(204/255.0, 128/255.0,   0/255.0),
  vec3(153/255.0,  87/255.0,   0/255.0),
  vec3(106/255.0,  52/255.0,   3/255.0),
  vec3( 30/255.0,  66/255.0,  15/255.0),
  vec3(  7/255.0,  25/255.0,  26/255.0),
  vec3(  1/255.0,   9/255.0,  47/255.0),
  vec3(  4/255.0,   4/255.0,  73/255.0),
  vec3(  7/255.0,   0/255.0, 100/255.0),
  vec3( 44/255.0,  12/255.0, 138/255.0),
  vec3( 82/255.0,  24/255.0, 177/255.0),
  vec3(125/255.0,  57/255.0, 209/255.0),
  vec3(181/255.0, 134/255.0, 229/255.0),
  vec3(236/255.0, 211/255.0, 248/255.0),
  vec3(233/255.0, 241/255.0, 191/255.0),
  vec3(201/255.0, 248/255.0,  95/255.0),
  vec3(170/255.0, 255/255.0,   0/255.0),
  vec3(128/255.0, 204/255.0,   0/255.0),
  vec3( 87/255.0, 153/255.0,   0/255.0),
  vec3( 52/255.0, 106/255.0,   3/255.0),
  vec3( 66/255.0,  15/255.0,  30/255.0),
  vec3( 25/255.0,  26/255.0,   7/255.0),
  vec3(  9/255.0,  47/255.0,   1/255.0),
  vec3(  4/255.0,  73/255.0,   4/255.0),
  vec3(  0/255.0, 100/255.0,   7/255.0),
  vec3( 12/255.0, 138/255.0,  44/255.0),
  vec3( 24/255.0, 177/255.0,  82/255.0),
  vec3( 57/255.0, 209/255.0, 125/255.0),
  vec3(134/255.0, 229/255.0, 181/255.0),
  vec3(211/255.0, 248/255.0, 236/255.0),
  vec3(241/255.0, 191/255.0, 233/255.0),
  vec3(248/255.0,  95/255.0, 201/255.0),
  vec3(255/255.0,   0/255.0, 170/255.0),
  vec3(204/255.0,   0/255.0, 128/255.0),
  vec3(153/255.0,   0/255.0,  87/255.0),
  vec3(106/255.0,   3/255.0,  52/255.0),
  vec3( 30/255.0,  15/255.0,  66/255.0),
  vec3(  7/255.0,  26/255.0,  25/255.0),
  vec3(  1/255.0,  47/255.0,   9/255.0),
  vec3(  4/255.0,  73/255.0,   4/255.0),
  vec3(  7/255.0, 100/255.0,   0/255.0),
  vec3( 44/255.0, 138/255.0,  12/255.0),
  vec3( 82/255.0, 177/255.0,  24/255.0),
  vec3(125/255.0, 209/255.0,  57/255.0),
  vec3(181/255.0, 229/255.0, 134/255.0),
  vec3(236/255.0, 248/255.0, 211/255.0),
  vec3(233/255.0, 191/255.0, 241/255.0),
  vec3(201/255.0,  95/255.0, 248/255.0),
  vec3(170/255.0,   0/255.0, 255/255.0),
  vec3(128/255.0,   0/255.0, 204/255.0),
  vec3( 87/255.0,   0/255.0, 153/255.0),
  vec3( 52/255.0,   3/255.0, 106/255.0),
};

out vec4 frag_color;

// f(z) = z*z + c, z starts as 0, c is some complex number
// f(0) = c
// f(f(0)) = c*c + c
// ...
// it does not diverte(remains bounded in absolute value)
float mandelbrot_set(dvec2 st)
{
  // use st as complex plane(x as real, y as imaginary)
  double ca = st.x;
  double cb = st.y;

  // firt iterations
  double za = st.x;
  double zb = st.y;

  // NUM_ITERATOIONS - 1 iterations
  float i = 0;
  while (++i < MAX_ITERATIONS) 
  {
    double a = za*za - zb*zb;
    double b = 2 * za * zb;
    za = a + ca;
    zb = b + cb;

    if((za*za + zb*zb) > BOUNDARY2)
      break;
  }

  if(i < MAX_ITERATIONS)
  {
    float log_zn = log(float(za*za + zb*zb)) * 0.5f;
    float nu = log(log(log_zn/log(2.0f)))/log(2.0f);
    i += 1.0f - nu;
  }

  //float i;
  return i;
}

void main(void)
{
  dvec2 st = gl_FragCoord.xy / resolution;

  float iterations = NUM_COLORS * mandelbrot_set(mix(start, end, st))/MAX_ITERATIONS;
  int c0 = int(floor(iterations)) % NUM_COLORS;
  int c1 = (c0+1) % NUM_COLORS;
  vec3 color = mix(colors[c0], colors[c1], fract(iterations));
  frag_color = vec4(color, 1);
  
}
