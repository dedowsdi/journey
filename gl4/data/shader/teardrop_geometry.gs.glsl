#version 430 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) out vec3 w_vertex;
layout(location = 1) out vec3 w_normal;

layout(location = 0) uniform mat4 m_mat;
layout(location = 1) uniform mat4 m_mat_it;

void main(void)
{
  w_normal = cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
      gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);
  w_normal = normalize(mat3(m_mat_it) * w_normal);

  gl_Position = m_mat * gl_in[0].gl_Position;
  w_vertex = gl_Position.xyz;
  EmitVertex();

  gl_Position = m_mat * gl_in[1].gl_Position;
  w_vertex = gl_Position.xyz;
  EmitVertex();

  gl_Position = m_mat * gl_in[2].gl_Position;
  w_vertex = gl_Position.xyz;
  EmitVertex();

  EndPrimitive();
}
