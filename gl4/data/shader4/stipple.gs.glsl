#version 430 core

// 
// reset stiple counter for each line segment

layout(lines) in;
layout(line_strip, max_vertices = 1024) out;

uniform ivec4 viewport;
noperspective out float stipple_count;

void main(void) {

  /*
   * real viewport vertex should be :
   * viewport.width * 0.5 * (gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w + 1) , 
   * but only length is needed, so +1 can be omitted, *0.5 can be applied later
   */
  vec2 vp_vertex0 =
    viewport.z * gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
  vec2 vp_vertex1 =
    viewport.z * gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
  gl_Position = gl_in[0].gl_Position;
  stipple_count = 0.0;
  EmitVertex();
  gl_Position = gl_in[1].gl_Position;
  stipple_count = 0.5 * length(vp_vertex1 - vp_vertex0); //don't omit 0.5, ndc is [-1, 1]
  EmitVertex();
}
