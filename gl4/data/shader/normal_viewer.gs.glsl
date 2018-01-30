#version 330 core

// don't want to see normal of backspace, so i  draw triangles instead of
// points, this also means if a vertex is shared by multiples triangles, it's
// normal will be drawn multiple times

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

uniform mat4 p_mat;
uniform float normal_length = 0.1;

in vs_out 
{ 
  vec3 normal; 
} gs_in[];

void main(void) {

  vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

  //ignore back face. 
  if(dot(cross(ab,ac), vec3(0, 0, 1)) < 0)
      return;

  for (int i = 0; i < 3; ++i) {

    gl_Position = p_mat * gl_in[i].gl_Position;
    EmitVertex();

    vec4 vertex = gl_in[i].gl_Position;
    vertex.xyz += gs_in[i].normal * normal_length * vertex.w;

    gl_Position = p_mat * vertex;
    EmitVertex();

    EndPrimitive();
  }
}
