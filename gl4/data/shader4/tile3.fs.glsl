#version 430 core
uniform vec2 resolution;
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


// offset odd row
vec2 brick_tile(vec2 st, vec2 times,  float offset)
{
  st *= times;
  st.x += -step(1.0, mod(st.y, 2)) * offset;
  return fract(st);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st /= vec2(2.15, 0.65)/1.5;
  st = brick_tile(st, vec2(5), 0.5);

  vec3 bg_color = vec3(0);
  vec3 brick_color = vec3(1);

  vec3 color = mix(bg_color, brick_color, rect(st, vec2(0.9), 0.01));
  frag_color = vec4(color, 1);
}
