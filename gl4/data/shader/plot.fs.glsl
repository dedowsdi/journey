#version 430 core

uniform vec2 resolution;
uniform int mode;
uniform float exponent;
uniform float line_width;
out vec4 frag_color;
const float PI = 3.1415926;

// f(x/stretch.x - translation.x) * stretch.y + translation.y
uniform vec2 translation;
uniform vec2 stretch;

// y : curve for current pront (st.x, y)
// return 0(background) to 1(curve)
float plot(vec2 st, float y)
{
  // smoothstep(y - w, y, st.y) = 0,      st.y <= y - w
  float w = line_width / 2; //  half width
  return smoothstep(y - w, y, st.y) - smoothstep(y, y + w, st.y);

  // unsmooth version
  //return 1 - min(abs(st.y - y), w)/w;
}

float transform_x(float x)
{
  return x / stretch.x - translation.x;
}

float transform_y(float y)
{
  return y * stretch.y + translation.y;
}

float curve0(float x)
{
  return x;
}

float curve1(float x)
{
  return 1 - pow(abs(x), exponent);
}

float curve2(float x)
{
  return pow(cos(PI * x / 2.0), exponent);
}

float curve3(float x)
{
  return 1- pow(abs(sin(PI * x / 2.0)), exponent);
}

float curve4(float x)
{
  return pow(min(cos(PI*x/2.0), 1-abs(x)), exponent);
}

float curve5(float x)
{
  return 1 - pow(max(0, abs(x) * 2.0 - 1.0), exponent);
}

float curve(float x)
{
  x = transform_x(x);
  float y;
  switch(mode)
  {
    case 0:
      y =  curve0(x);
      break;
    case 1:
      y =  curve1(x);
      break;
    case 2:
      y =  curve2(x);
      break;
    case 3:
      y =  curve3(x);
      break;
    case 4:
      y =  curve4(x);
      break;
    case 5:
      y =  curve5(x);
      break;
  }
  return transform_y(y);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  float y = curve(st.x);

  vec3 color = vec3(y);

  //plot a line
  float pct = plot(st, y);
  color = (1.0-pct)*color+pct*vec3(0.0,1.0,0.0);
  // if you want, you can continue draw other lines:
  // color = mix(color, new graph color, new graph pct);

  frag_color = vec4(color, 1);
}
