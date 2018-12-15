//#version 430 core

#ifndef NUM_EYEBALLS
#define NUM_EYEBALLS 1
#endif

in vs_out
{
  vec2 texcoord;
}fi;

struct eyeball
{
  float r0;
  float r1;
  vec2 pos;
  vec2 pupil_pos;
};

layout(location = 0)uniform sampler2D quad_map;
layout(location = 1)uniform vec2 resolution;
layout(location = 2)uniform eyeball eyeballs[NUM_EYEBALLS];

out vec4 frag_color;

vec2 original_texcoord(eyeball ball, vec2 st)
{
  // outer area
  vec2 o_to_st = st - ball.pos;
  if(dot(o_to_st, o_to_st) >= ball.r1 * ball.r1)
    return st;

  vec2 o_to_pupil = ball.pupil_pos - ball.pos;

  // pupil area
  vec2 to_pupil = ball.pupil_pos - st;
  if(dot(to_pupil, to_pupil) <= ball.r0 * ball.r0)
    return st - o_to_pupil;

  // middle area
  // solve a*|C| = b + normalized_C
  float r2 = ball.r1 - ball.r0;
  vec2 a = o_to_st - ball.r1/r2 * o_to_pupil;
  float a_length = length(a);
  a /= a_length;
  vec2 b = -o_to_pupil / r2;

  vec2 b_on_a = dot(b,a) * a;
  float dist2_a_b = dot(b,b) - dot(b_on_a, b_on_a);
  vec2 b2b_on_a = b_on_a - b;
  vec2 u = b2b_on_a + a * sqrt(1 - dist2_a_b);

  // |C| = |b + normalized_C| / |a|
  return ball.pos + u * a_length / length(u + b);

  // another solution is u1 = b2b_on_a - a * sqrt(1 - dist2_a_b);
  // this happens when length(b) > 1, then you got another C, you need test it
  // to make sure which one is right. 
  //  C1 = u1 * a_length / length(u + b);
  //  dot0 = dot(normalize(C1 - O'), normalized O')
  //  dot1 = dot(normalize(C1 - O'), normalized O')
  //
  //  the smaller one is our solution. 
  //  I don't know why i don't need this test, everying just works fine.
}


void main(void)
{
  vec2 st = gl_FragCoord.xy;
  vec2 ost;

  for (int i = 0; i < NUM_EYEBALLS; i++) {
    ost = original_texcoord(eyeballs[i], st);
    if(ost != st) break;
  }
  st = ost / resolution;

  frag_color = texture(quad_map, st);
}
