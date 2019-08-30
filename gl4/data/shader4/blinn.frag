#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif

struct light_source {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 position;  
  vec3 spot_direction;
  float spot_exponent;
  float spot_cutoff;
  float spot_cos_cutoff;
  float constant_attenuation;
  float linear_attenuation;
  float quadratic_attenuation;
};

struct light_model{
  bool local_viewer; // only has meaning if lighting is in view space
  vec4 ambient;
};

struct light_product {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};

struct material {
  vec4 emission;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

void light(vec3 vertex, vec3 normal, vec3 v2e, light_source source,
  material mtl, out light_product product) {
  product = light_product(vec4(0), vec4(0), vec4(0));
  // skip blank light
  if (source.constant_attenuation == 0.0) return;

  vec3 v2l = source.position.w == 0.0
               ? source.position.xyz
               : source.position.xyz / source.position.w - vertex;
  float dist = length(v2l);
  vec3 l = v2l / dist;
  vec3 n = normal;
  vec3 v = v2e;

  // spot light effect
  float spot_effect = 1.0;
  if (source.position.w == 1.0 && source.spot_cutoff != 180.0) {
    float nl_dot_d = dot(-l, source.spot_direction);
    if (nl_dot_d < source.spot_cos_cutoff) return;
    spot_effect = pow(clamp(nl_dot_d, 0.0, 1.0), source.spot_exponent);
  }

  // attenuation
  float atten = 1.0; 
  if(source.position.w == 1.0)
  {
    atten = 1.0 / (source.constant_attenuation + source.linear_attenuation *
        dist + source.quadratic_attenuation * dist * dist);
  }

  float visibility = atten * spot_effect;

  // ambient
  product.ambient = visibility * source.ambient * mtl.ambient;

  // diffuse
  float ndotl = dot(n, l);

  // should i comment this ? it cause specular beak in model edge sometimes
  if (ndotl <= 0.0) return;

  product.diffuse = visibility * ndotl * source.diffuse * mtl.diffuse; 

  // specular
  vec3 h = normalize(l + v);
  float ndoth = dot(h, n);
  if(ndoth <= 0.0) return;

  product.specular = visibility * pow(ndoth, mtl.shininess) * source.specular * mtl.specular;
}

// split diffuse and specular
void blinn(vec3 vertex, vec3 normal, vec3 camera, material mtl, light_source
    lights[LIGHT_COUNT], light_model lm, out vec4 diffuse, out vec4 specular){
  // if you calculating lighting in non-view space, make sure lcoal_viewer is 1
  vec3 v2e = lm.local_viewer ? normalize(camera - vertex) : vec3(0,0,1);
  light_product product = light_product(vec4(0),vec4(0),vec4(0));

  for (int i = 0; i < LIGHT_COUNT; i++) {
    light_product currentProduct = light_product(vec4(0),vec4(0),vec4(0));
    light(vertex, normal, v2e, lights[i], mtl, currentProduct);
    product.diffuse += currentProduct.diffuse;
    product.specular += currentProduct.specular;
    product.ambient += currentProduct.ambient;
  }

  diffuse = mtl.emission + lm.ambient * mtl.ambient +
    product.ambient + product.diffuse;
  specular = product.specular;
}

vec4 blinn(vec3 vertex, vec3 normal, vec3 camera, material mtl, light_source
    lights[LIGHT_COUNT], light_model lm){
  vec4 diffuse;
  vec4 specular;
  blinn(vertex, normal, camera, mtl, lights, lm, diffuse, specular);
  vec4 color = diffuse + specular;
  color.a = mtl.diffuse.a;
  return color;
}
