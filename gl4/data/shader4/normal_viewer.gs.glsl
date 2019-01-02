//#version 330 core

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
} gi[];

void main(void) {


  gl_Position = p_mat * gl_in[0].gl_Position;
  EmitVertex();
  vec4 vertex = gl_in[0].gl_Position;
  vertex.xyz += gi[0].normal * normal_length * vertex.w;
  EmitVertex();

  EndPrimitive();

#ifndef SMOOTH_NORMAL
  vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  //ignore back face. 
  if(dot(cross(ab,ac), vec3(0, 0, 1)) < 0.000001)
    return;
#endif

  for (int i = 0; i < 3; ++i) {

#ifdef SMOOTH_NORMAL
  if(dot(normalize(gi[i].normal), vec3(0, 0, 1)) < 0)
    return;
#endif

    gl_Position = p_mat * gl_in[i].gl_Position;
    EmitVertex();

    vec4 vertex = gl_in[i].gl_Position;
    vertex.xyz += gi[i].normal * normal_length * vertex.w;

    gl_Position = p_mat * vertex;
    EmitVertex();

    EndPrimitive();
  }

}
