#version 430 core

struct material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emission;
  float shininess;
};

struct light_source
{
  vec4 position;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emission;
  vec3 attenuation;
  vec3 spot_direction;
  float spot_cutoff;
  float cos_spot_cutoff;
  float spot_exponent;
};

struct light_model
{
  bool local_viewer;
  vec3 v2e;
  vec4 ambient;
};

struct light_product
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};

struct separate_color
{
  vec4 diffuse;
  vec4 specular;
};

#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif /* ifndef LIGHT_COUNT */

light_product blinn_single_light(
  vec3 vertex, vec3 normal, vec3 v2e, light_source light, material m)
{
  light_product res = {vec4(0), vec4(0), vec4(0)};
  vec3 v2l;
  float attenuation;
  if (light.position.w == 0)
  {
    v2l = normalize(light.position.xyz);
    attenuation = 1;
  }
  else
  {
    v2l = light.position.xyz / light.position.w - vertex;
    float lv_dist = length(v2l);
    attenuation = 1 / (light.attenuation[0] + lv_dist * light.attenuation[1] +
                        lv_dist * lv_dist * light.attenuation[2]);
    v2l /= lv_dist;

    // spot effect
    if (light.spot_cutoff != 180)
    {
      float sdir_dot_l = dot(-v2l, light.spot_direction);
      float spot_effect = sdir_dot_l > light.cos_spot_cutoff
                            ? pow(sdir_dot_l, light.spot_exponent)
                            : 0;
      attenuation *= spot_effect;
    }
  }

  // ambient
  res.ambient = light.ambient * m.ambient * attenuation;

  // diffuse
  float n_dot_l = max(dot(normal, v2l), 0.0);

  // cause problem in model edge for positional light when shininess is low. But
  // glspec2.1 says if n_dot_l is 0, there should be no specular lighting,
  // something must be wrong.
  // // if(n_dot_l == 0.0)
  //   return res;

  res.diffuse = light.diffuse * m.diffuse * n_dot_l * attenuation;

// specular
#ifdef PHONG
  vec3 r = normalize(reflect(-v2l, normal));
  float specular_factor = max(dot(v2e, r), 0.0f);
#else
  vec3 h = normalize(v2e + v2l);
  float specular_factor = max(dot(normal, h), 0.0f);
#endif

  if (specular_factor == 0.0f) return res;

  res.specular = light.specular * m.specular * attenuation *
                 pow(specular_factor, m.shininess);
  return res;
}

light_product blinn(vec3 vertex, vec3 normal, vec3 eye, light_model lm,
  light_source[LIGHT_COUNT] lights, material m)
{
  light_product product;
  product.ambient = lm.ambient * m.ambient;
  product.diffuse = m.emission;
  product.specular = vec4(0);
  vec3 v2e = lm.local_viewer ? normalize(eye - vertex) : lm.v2e;

  for (int i = 0; i < LIGHT_COUNT; i++)
  {
    light_product p = blinn_single_light(vertex, normal, v2e, lights[i], m);
    product.ambient += p.ambient;
    product.diffuse += p.diffuse;
    product.specular += p.specular;
  }

  return product;
}

vec4 blinn_single(vec3 vertex, vec3 normal, vec3 eye, light_model lm,
  light_source[LIGHT_COUNT] lights, material m)
{
  light_product product = blinn(vertex, normal, eye, lm, lights, m);
  vec4 color = product.ambient + product.diffuse + product.specular;
  color.a = m.diffuse.a;
  return color;
}

separate_color blinn_separate(vec3 vertex, vec3 normal, vec3 eye,
  light_model lm, light_source[LIGHT_COUNT] lights, material m)
{
  light_product product = blinn(vertex, normal, eye, lm, lights, m);
  vec4 diffuse = product.ambient + product.diffuse;
  diffuse.a = m.diffuse.a;
  return separate_color(diffuse, product.specular);
}

uniform light_model lm;
uniform material mtl;
uniform light_source lights[LIGHT_COUNT];
