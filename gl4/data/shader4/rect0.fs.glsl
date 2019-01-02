#version 430 core

uniform vec2 resolution;
out vec4 frag_color;

// some equation take x=0.5 as axis of symmetry.
// it should increase if x belongs to [0, 0.5]
//           decrease if x belongs to [0.5, 1]
vec2 rect_test(vec2 st)
{
  return st * (1 - st);
}

// create rect in screen center. if you want to create it in other position,
// apply scale or translation to st
float rect(vec2 st, vec2 size, float smoothedge)
{
  vec2 t0 = rect_test(st);
  vec2 t1 = rect_test((1-size)/2);
  //vec2 uv = step(t1, t0);
  vec2 uv = smoothstep(t1, t1+0.00001, t0);
  return uv.x * uv.y;
}

float rect_ring(vec2 st, vec2 inner_size, vec2 outer_size, float smoothedge)
{
  float t0 = rect(st, inner_size, smoothedge);
  float t1 = rect(st, outer_size, smoothedge);
  return t1 * (1-t0);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy/resolution;

  vec3 background_color0 = vec3(0.0, 0.5, 1.0);
  vec3 background_color1 = vec3(0.0, 0.0, 0.0);
  vec3 rect_color0 = vec3(1.0, 0.0, 0.0);
  vec3 rect_color1 = vec3(0.0, 1.0, 0.0);

  vec3 color = mix(background_color0, background_color1, st.y);
  color = mix(color, rect_color0, rect(st, vec2(0.125, 0.6), 0.001));

  // use st and offset to control rect position
  vec2 offset = vec2(0.2, 0);
  color = mix(color, rect_color0, rect(st - offset, vec2(0.125, 0.6), 0.001));
  color = mix(color, rect_color0, rect(st + offset, vec2(0.125, 0.6), 0.001));

  color = mix(color, rect_color1, rect((st - vec2(0, 0.5)) * vec2(1, 2),
        vec2(0.8, 0.125) * vec2(1.0, 2.0), 0.001));
  color = mix(color, rect_color1, rect(st * vec2(1, 2),
        vec2(0.8, 0.125) * vec2(1.0, 2.0), 0.001));

  color = mix(color, rect_color0, rect_ring(st, vec2(0.8), vec2(0.9), 0.001));

  frag_color = vec4(color, 1);
}
