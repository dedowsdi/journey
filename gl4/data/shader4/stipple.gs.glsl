#version 430 core

layout(lines) in;
layout(line_strip, max_vertices = 2) out;

in vertex_data
{
  vec4 color;
} gi[];

layout(location = 1) uniform ivec4 vp;

out vertex_data
{
  vec4 color;
  noperspective float stipple_count;
} go;

void main(void)
{
  vec4 pos0 = gl_in[0].gl_Position;
  vec4 pos1 = gl_in[1].gl_Position;

  // clip to viewport
  vec2 vpos0 = (pos0.xy/pos0.w + 1.0) * 0.5 * vp.zw;
  vec2 vpos1 = (pos1.xy/pos1.w + 1.0) * 0.5 * vp.zw;

  gl_Position = pos0;
  go.color = gi[0].color;
  go.stipple_count = 0;
  EmitVertex();

  gl_Position = pos1;
  go.color = gi[1].color;
  go.stipple_count = length(vpos1 - vpos0);
  EmitVertex();

  EndPrimitive();
}
