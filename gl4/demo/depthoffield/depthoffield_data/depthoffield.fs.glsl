#version 430 core

in vs_out
{
  vec2 texcoord;
} fi;

layout(location = 0) uniform sampler2D color_map;
layout(location = 1) uniform sampler2D depth_map;
layout(location = 2) uniform sampler2D blur_map;
layout(location = 3) uniform vec2 near_far;
layout(location = 4) uniform vec2 field; // start distance, range

out vec4 frag_color;

float depth_to_eye_z(float depth, float n, float f)
{
  // assume depth in 0-1
  float ndc_z = depth * 2.0f - 1.0f;
  return 2 * n * f / (ndc_z * (f - n) - f - n);
}

void main(void)
{
  vec4 color = texture(color_map, fi.texcoord);
  vec4 blur = texture(blur_map, fi.texcoord);
  float depth = texture(depth_map, fi.texcoord).x;
  float distance = -depth_to_eye_z(depth, near_far.x, near_far.y);
  float blend = clamp((distance - field.x) / (field.y), 0.0f, 1.0f);
  frag_color = color * (1 - blend) + blur * blend;
}
