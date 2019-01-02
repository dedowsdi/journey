#version 330 core
#define PI 3.1415926535897932384626433832795

#define N 3
#define D 1

int gcd(int x, int y)
{
  if(x == y)
    return x;

  int a = x;
  int b = y;

  if(a < b)
  {
    a = y;
    b = x;
  }

  while(true)
  {
    int c = a%b;
    if(c == 0)
      return b;
    a = b;
    b = c;
  }
}


uniform vec2 resolution;
out vec4 frag_color;

float rose(vec2 st)
{
  float k = float(N) / D;
  int g = gcd(N, D);
  int n = N/g;
  int d = D/g;
  int t = d;
  if((n & 1) == 0)
    t *= 2;

  float theta = atan(st.y, st.x);
  float r = abs(cos(k*theta));
  float l = length(st);
  return smoothstep(l-0.02, l, r) * smoothstep(r-0.02, r, l);
  //return smoothstep(l-0.03, l, r);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st = st * 2.0 - 1.0;
  st*= 1.1;

  vec3 bg_color = vec3(0);
  vec3 fg_color = vec3(1);

  vec3 color = mix(bg_color, fg_color, rose(st));
  frag_color = vec4(color, 1);
}
