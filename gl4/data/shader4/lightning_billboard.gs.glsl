#version 430 core

#define PI 3.1415926535897932384626433832795
#define TWO_PI (3.1415926535897932384626433832795*2)

layout(lines) in;
layout(triangle_strip, max_vertices = 100) out;

layout(location = 0) uniform float billboard_width = 2;
layout(location = 1) uniform int gap_slices = 6;
layout(location = 2) uniform vec3 camera_pos;
layout(location = 3) uniform mat4 mvp_mat;

out vs_out
{
  vec2 texcoord;
}go;

void emit_vertex(vec4 pos, vec2 texcoord)
{
  gl_Position = pos;
  go.texcoord = texcoord;
  EmitVertex();
}

void main(void)
{
  float radius = billboard_width * 0.5f;
  float step_angle = TWO_PI / gap_slices;
  int half_slice = int(gap_slices * 0.5);

  vec3 lv0 = gl_in[0].gl_Position.xyz;
  vec3 lv1 = gl_in[1].gl_Position.xyz;
  vec3 v01 = normalize(lv1 - lv0);

  vec3 ev = camera_pos - 0.5f*(lv0+lv1);
  // constrain billoboards rotate to v01 only; after rototation, side should
  // be perp to both ev and v01
  vec3 side = radius * normalize(cross(v01, ev));

  vec4 v0 = mvp_mat * vec4(lv0 - side, 1);
  vec4 v1 = mvp_mat * vec4(lv0 + side, 1);
  vec4 v2 = mvp_mat * vec4(lv1 + side, 1);
  vec4 v3 = mvp_mat * vec4(lv1 - side, 1);

  vec2 tc0 = vec2(0);
  vec2 tc1 = vec2(1, 0);
  vec2 tc2 = vec2(1, 1);
  vec2 tc3 = vec2(0, 1);

  emit_vertex(v0, tc0);
  emit_vertex(v1, tc1);
  emit_vertex(v2, tc2);
  EndPrimitive();

  emit_vertex(v0, tc0);
  emit_vertex(v2, tc2);
  emit_vertex(v3, tc3);
  EndPrimitive();


  // close gap with semicircle
  vec3 up = radius * v01;
  // bottom
  vec4 bottom_center = mvp_mat * vec4(lv0, 1);
  vec2 bottom_center_texcoord = vec2(0.5);
  vec4 last_second = v0;
  vec2 last_second_texcoord = vec2(0, 0.5);

  for (int j = 1; j <= half_slice; ++j) {
    float angle = step_angle * j;
    float c = cos(angle);
    float s = sin(angle);
    emit_vertex(bottom_center, bottom_center_texcoord);
    emit_vertex(last_second, last_second_texcoord);
    vec4 pos = mvp_mat * vec4(lv0 - side * c - up * s, 1);
    vec2 texcoord = vec2(0.5 - 0.5*c, 0.5 - 0.5*s);
    emit_vertex(pos, texcoord);
    EndPrimitive();
    last_second = pos;
    last_second_texcoord = texcoord;
  }

  // top
  vec4 top_center = mvp_mat * vec4(lv1, 1);
  vec2 top_center_texcoord = vec2(0.5);
  last_second = v2;
  last_second_texcoord = vec2(1, 0.5);
  for (int j = 1; j <= half_slice; ++j) {
    float angle = step_angle * j;
    float c = cos(angle);
    float s = sin(angle);
    emit_vertex(top_center, top_center_texcoord);
    emit_vertex(last_second, last_second_texcoord);
    vec4 pos = mvp_mat * vec4(lv1 + side * c + up * s, 1);
    vec2 texcoord = vec2(0.5 + 0.5*c, 0.5 + 0.5*s);
    emit_vertex(pos, texcoord);
    EndPrimitive();
    last_second = pos;
    last_second_texcoord = texcoord;
  }
}
