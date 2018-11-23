#version 430 core
#define PI 3.1415926535897932384626433832795
#define TWO_PI (PI * 2)
#define PI4 (PI * 0.25)

layout(lines) in;
layout(line_strip, max_vertices = 6) out;

layout(location = 0) uniform int pattern = 0; // 0 jitter, 1 fork
layout(location = 1) uniform float max_jitter = 0.5f;
layout(location = 2) uniform float max_fork_angle = PI4;
layout(location = 3) uniform float random_seed = 0;
//layout(location = 4) uniform vec3 global_dir = vec3(1, 0, 0);

out vec3 pos;

float random(float f)
{
  return fract(sin(f + random_seed + gl_PrimitiveIDIn) * 10000);
}

float linearRand(float a, float b, float seed)
{
  return mix(a, b, random(seed));
}

vec3 sphericalRand(float radius, float seed)
{
  float phi = random(seed) * PI;
  float theta = random(seed) * TWO_PI;
  float cos_phi_r = cos(phi)*radius;
  float sin_phi_r = sin(phi)*radius;
  return vec3(sin_phi_r * cos(theta), sin_phi_r * sin(theta), cos_phi_r);
}

vec3 random_orthogonal(vec3 dir, float seed)
{
  // this guard would not work!!!!!
  //if(dot(dir, dir) < 0.001)
    //return vec3(1, 0, 0);

  int times = 6;
  vec3 ref = sphericalRand(1.0, seed);
  while(abs(dot(dir, ref)) > 0.99999f && --times > 0)
  {
    seed += 100;
    ref = sphericalRand(1.0, seed);
  }
  return normalize(cross(dir, ref));
}

vec3 rotate_in_cone(vec3 ndir, float angle, float seed)
{
  return ndir * cos(angle) + random_orthogonal(ndir, seed) * sin(angle);
}

void main(void)
{
  if(pattern > 1)
    return;

  vec3 v0 = gl_in[0].gl_Position.xyz;
  vec3 v1 = gl_in[1].gl_Position.xyz;

  pos = v0;
  EmitVertex();

  float seg_length = 0.5f * distance(v0, v1);

  // jitter
  vec3 v2 = (v0 + v1) * 0.5f;
  vec3 v01 = v1 - v0;

  v2 += random_orthogonal(normalize(v01), 200) * linearRand(0, max_jitter, 300) * seg_length;
  pos = v2;
  EmitVertex();
  EndPrimitive();

  pos = v2;
  EmitVertex();
  pos = v1;
  EmitVertex();
  EndPrimitive();

  // fork
  if(pattern == 1)
  {
    vec3 dir = normalize(v1 - v0);
    // fork len should be the same as normal segment
    float seg_len = distance(v0, v2);
    float theta = linearRand(-max_fork_angle, max_fork_angle, 400);
    vec3 v3 = v2 + rotate_in_cone(dir, theta, 500) * seg_len;
    pos = v2;
    EmitVertex();
    pos = v3;
    EmitVertex();
    EndPrimitive();
  }
}
