//http://www.karlsims.com/rd.html
//
#version 330 core

uniform float da = 1;
uniform float db = 0.5;
uniform float feed = 0.055;
uniform float kill = 0.062;
uniform float delta_time = 1;

in VS_OUT
{
  vec2 texcoord;
}fi;

uniform sampler2D diffuse_map;

out vec4 frag_color;

float laplacian_a(vec2 t, vec2 uv_step)
{
  vec2 u_step = vec2(uv_step.x, 0);
  vec2 v_step = vec2(0, uv_step.y);

  float a = 0;
  a += -1 * texture(diffuse_map, t).r; // center
  a += 0.2 * texture(diffuse_map, t + u_step).r; // cross
  a += 0.2 * texture(diffuse_map, t - u_step).r;
  a += 0.2 * texture(diffuse_map, t + v_step).r;
  a += 0.2 * texture(diffuse_map, t - v_step).r;
  a += 0.05 * texture(diffuse_map, t + uv_step).r; // corner
  a += 0.05 * texture(diffuse_map, t - uv_step).r;
  a += 0.05 * texture(diffuse_map, t + u_step - v_step).r;
  a += 0.05 * texture(diffuse_map, t - u_step + v_step).r;

  return a;
}

float laplacian_b(vec2 t, vec2 uv_step)
{
  vec2 u_step = vec2(uv_step.x, 0);
  vec2 v_step = vec2(0, uv_step.y);

  float b = 0;
  b += -1 * texture(diffuse_map, t).g; // center
  b += 0.2 * texture(diffuse_map, t + u_step).g; // cross
  b += 0.2 * texture(diffuse_map, t - u_step).g;
  b += 0.2 * texture(diffuse_map, t + v_step).g;
  b += 0.2 * texture(diffuse_map, t - v_step).g;
  b += 0.05 * texture(diffuse_map, t + uv_step).g; // corner
  b += 0.05 * texture(diffuse_map, t - uv_step).g;
  b += 0.05 * texture(diffuse_map, t + u_step - v_step).g;
  b += 0.05 * texture(diffuse_map, t - u_step + v_step).g;

  return b;
}

void main(void)
{
  vec4 color = texture(diffuse_map, fi.texcoord);
  float a = color.r;
  float b = color.g;
  float abb = a*b*b;

  ivec2 tsize = textureSize(diffuse_map, 0);
  vec2 uv_step = vec2(1.0) / tsize;

  frag_color.r =  a + (da * laplacian_a(fi.texcoord, uv_step) - abb + feed * (1-a)) * delta_time;
  frag_color.g =  b + (db * laplacian_b(fi.texcoord, uv_step) + abb - (kill+feed)*b) * delta_time;
}
