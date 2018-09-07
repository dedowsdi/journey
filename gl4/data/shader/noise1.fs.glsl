#version 430 core

#define PI 3.1415926535897932384626433832795

uniform vec2 resolution;
uniform float time;
out vec4 frag_color;

vec2 rect_test(vec2 st)
{
  return st * (1 - st);
}

// create rect in screen center.
float rect(vec2 st, vec2 size, float smoothedge)
{
  vec2 t0 = rect_test(st);
  vec2 t1 = rect_test((1-size)/2);
  //vec2 uv = step(t0, t1);
  vec2 uv = smoothstep(t1, t1+smoothedge, t0);
  return uv.x * uv.y;
}

float random(vec2 st)
{
  return fract(sin(dot(st, vec2(12.9898,78.233))) * 43758.5453123);
}

float random(float f)
{
  return fract(sin(f) * 10000);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;

  st.x += (noise(st * time * 5) - 0.5) * 0.02;
  st.y += (noise(st * time * 5 * sin(time)) - 0.5) * 0.04;

  vec3 bg_color = vec3(1);
  vec3 fg_color = vec3(0);

  vec3 color = mix(bg_color, fg_color, rect(st, vec2(0.6), 0.001));

  frag_color = vec4(color, 1);
}
