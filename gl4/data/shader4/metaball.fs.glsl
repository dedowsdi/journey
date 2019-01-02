/*
 * determine color by isosurface:
 *   color_precent = radius / length(ballcenter - st);
 *  
 */
#version 330 core

#define NUM_BALLS 100
#define MIN_RADIUS 0.01
#define MAX_RADIUS 0.05

uniform vec2 resolution;
uniform float time;

out vec4 frag_color;

float random(float f)
{
  return fract(sin(f) * 10000);
}

float linearRand(float seed, float minValue, float maxValue)
{
  return minValue + random(seed) * (maxValue - minValue);
}

vec2 linearRand(vec2 seed, vec2 minValue, vec2 maxValue)
{
  return vec2(linearRand(seed.x, minValue.x, maxValue.x), 
              linearRand(seed.y, minValue.y, maxValue.y));
}

struct Ball
{
  float radius;
  vec2 center;
  vec2 ab; // ab of ellipse
};

vec3 ball0 = vec3(0.5, 0.5, 0.06);

float metaball(vec2 st, Ball ball)
{
  float pct = ball.radius / length(st - ball.center);
  return pct*pct;
  return pct;
}

vec3 hsb2rgb( in vec3 c ){
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                             6.0)-3.0)-1.0, 0.0, 1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix( vec3(1.0), rgb, c.y);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;

  Ball balls[NUM_BALLS];
  for (int i = 0; i < NUM_BALLS; i++) {
    float radius = linearRand(i * 1000, MIN_RADIUS, MAX_RADIUS);
    radius += sin(time*0.15)* linearRand(i*83, 0.01, 0.02);
    vec2 ab = linearRand(vec2(i*5000, i*8000), vec2(0.1), vec2(0.35));
    ab.x += cos(time*3) * linearRand(i*38, 0.01, 0.1);
    ab.y += sin(time*3) * linearRand(i*92, 0.01, 0.1);
    vec2 pos = linearRand(vec2(i*2000, i*3000), vec2(0), vec2(1));
    pos.x += ab.x * cos(time * linearRand(313*i, 0.5, 1)) 
      * mix(-1, 1, step(0.5, random(i * 12313)));
    pos.y += ab.y * sin(time * linearRand(269*i, 0.1, 1)) 
      * mix(-1, 1, step(0.5, random(i * 413)));
    balls[i] = Ball(radius, pos, ab);
  }

  // animate balls
  vec2 velocity = vec2(random(1234), random(4321));

  vec3 bg_color = vec3(0);
  vec3 fg_color = vec3(1);

  float pct = 0;
  for (int i = 0; i < NUM_BALLS; i++) 
  {
    pct += metaball(st, balls[i]);
  }

  vec3 color = hsb2rgb(vec3(pct, 1, 1));

  frag_color = vec4(color, 1);
  
}
