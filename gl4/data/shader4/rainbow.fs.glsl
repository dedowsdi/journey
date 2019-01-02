#version 430 core

#define RAINBOW_WIDTH 0.06

uniform vec2 resolution;
const float PI = 3.1415926;

out vec4 frag_color;

const vec3 colors[7] = 
{
  {211, 0, 201},
  {122, 0, 229},
  {0, 104, 255},
  {0, 188, 63},
  {255, 255, 0},
  {255, 127, 0},
  {255, 0, 0}
};

float plot(vec2 st, float y)
{
  float w = RAINBOW_WIDTH / 2; //  half width
  //return smoothstep(y - w, y, st.y) - smoothstep(y, y + w, st.y);
  return abs(y - st.y) <= w ? 1 : 0;
}

float rainbow(float x, float translation)
{
  float exponent = 0.5;
  x = (x-0.5)/1.3;
  float y = pow(cos(PI * x / 2.0), exponent) + translation;

  return y;
}


void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;

  vec3 color = vec3(0, 0.5, 1);

  float translation = -0.5;
  for (int i = 0; i < 7; i++) 
  {
    float y = rainbow(st.x, translation);
    float pct = plot(st, y);
    color = mix(color, colors[i]/255.0, pct);
    translation += RAINBOW_WIDTH;
  }

  frag_color = vec4(color, 1);
}
