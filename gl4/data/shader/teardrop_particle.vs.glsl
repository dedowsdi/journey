#version 430 core

layout(location = 0)in vec3 pos;
layout(location = 1)in vec3 vel;

out vec3 pos_o;
out vec3 vel_o;

uniform samplerBuffer geometry_tbo;
uniform mat4 vp_mat;
uniform int triangle_count;

// orgin + direction = destination
bool intersect(vec3 origin, vec3 direction, vec3 v0, vec3 v1, vec3 v2, out vec3 point)
{
  vec3 v01 = v1 - v0;
  vec3 v02 = v2 - v0;
  vec3 pn = cross(v01, v02);
  float dot_pn_d = dot(pn, direction);
  if (abs(dot_pn_d) < 0.000001f) // parallel check
    return false;

  //float pd = -dot(v0, pn);
  //float t = (-pd - dot(origin, pn)) / dot_pn_d;
  float t = dot(v0 - origin, pn) / dot_pn_d;

  if(t < 0.0f || t > 1.0f )
    return false;

  point = origin + direction * t;

  // check point in triangle
  if(dot(pn, cross(v01, point - v0)) < 0)
    return false;
  if(dot(pn, cross(v2 - v1, point - v1)) < 0)
    return false;
  if(dot(pn, cross(-v02, point - v2)) < 0)
    return false;
  return true;
}

void main(void)
{
  vec3 point;
  pos_o = pos + vel;
  vel_o = vel;

  if(pos_o.z > 0.9f)
  {
    pos_o = pos;
    vel_o.z = - abs(vel_o.z);
  }
  else
  {
    for (int i = 0; i < triangle_count; ++i ) {
      vec3 v0 = texelFetch(geometry_tbo, i*3 + 0).xyz;
      vec3 v1 = texelFetch(geometry_tbo, i*3 + 1).xyz;
      vec3 v2 = texelFetch(geometry_tbo, i*3 + 2).xyz;

      //if(v0 == v1 || v0 == v2 || v1 == v2)
      //continue;

      if (intersect(pos_o, -vel, v0, v1, v2, point))
      {
        if(isnan(point.x) || isnan(point.y) || isnan(point.z))
          continue;

        vec3 n = normalize(cross(v1 - v0, v2 - v0));
        //pos_o = point + reflect(pos_o - point, -n); // cause leaking
        pos_o = point + 0.000001f * -n; // not accurate, but no leaking
        vel_o = 0.999f * reflect(vel, -n);
        break;
      }
    }
  }

  gl_Position = vp_mat * vec4(pos_o, 1);
}
