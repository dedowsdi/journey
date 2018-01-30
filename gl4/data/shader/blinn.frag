#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif

// exactly the same as gl__light_source_parameters
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
  vec4 ambient;
  bool local_viewer; // only has meaning if lighting is in view space
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

uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];
uniform material mtl;

void light(vec3 vertex, vec3 normal, vec3 v2e, light_source source,
  material mtl, out light_product product) {
  product = light_product(vec4(0), vec4(0), vec4(0));
  // skip blank light
  if (source.constant_attenuation == 0.0) return;

  vec3 v2l = source.position.w == 0.0 ? source.position.xyz : source.position.xyz - vertex;
  vec3 l = normalize(v2l);
  vec3 n = normal;
  vec3 v = v2e;

  // spot light effect
  float spotEffect = 1.0;
  if (source.position.w == 1.0 && source.spot_cutoff >= 0.0 &&
      source.spot_cutoff <= 90.0) {
    float nldotd = dot(-l, source.spot_direction);
    if (nldotd < source.spot_cos_cutoff) return;
    spotEffect = pow(clamp(nldotd, 0.0, 1.0), source.spot_exponent);
  }

  // attenuation
  float atten = 1.0; 
  if(source.position.w == 1.0)
  {
    float dist = length(v2l);
    atten = 1.0 / (source.constant_attenuation + source.linear_attenuation *
        dist + source.quadratic_attenuation * dist * dist);
  }

  float visibility = atten * spotEffect;

  // ambient
  product.ambient = visibility * source.ambient * mtl.ambient;

  // diffuse
  float ndotl = dot(n, l);
  if (ndotl <= 0.0) return;
  ndotl = clamp(ndotl, 0.0, 1.0);
  product.diffuse = visibility * ndotl * source.diffuse * mtl.diffuse; 

  // specular
  vec3 h = normalize(l + v);
  float ndoth = dot(h, n);
  if(ndoth <= 0.0) return;
  ndoth = clamp(ndoth, 0.0, 1.0);

  product.specular = visibility * pow(ndoth, mtl.shininess) * source.specular * mtl.specular;
}

light_product get_light_product(vec3 vertex, vec3 normal, vec3 camera, material mtl,
    light_source lights[LIGHT_COUNT]){
  vec3 v2e = lm.local_viewer ? normalize(camera - vertex) : vec3(0,0,1);
  light_product product = light_product(vec4(0),vec4(0),vec4(0));

  for (int i = 0; i < LIGHT_COUNT; i++) {
    light_product currentProduct = light_product(vec4(0),vec4(0),vec4(0));
    light(vertex, normal, v2e, lights[i], mtl, currentProduct);
    product.diffuse += currentProduct.diffuse;
    product.specular += currentProduct.specular;
    product.ambient += currentProduct.ambient;
  }
  return product;
}

// the most simple case, in view space
vec4 blinn(vec3 vertex, vec3 normal){
  light_product product = get_light_product(vertex, normal, vec3(0.0), mtl, lights);
  vec4 color = mtl.emission + lm.ambient * mtl.ambient +
    product.ambient + product.diffuse + product.specular;
  return color;
}

// some shader(such as gbuffer) get material from texture, in view space
vec4 blinn(vec3 vertex, vec3 normal, material mtl){
  light_product product = get_light_product(vertex, normal, vec3(0.0), mtl, lights);
  vec4 color = mtl.emission + lm.ambient * mtl.ambient +
    product.ambient + product.diffuse + product.specular;
  return color;
}

// some shader(such as normal map) don't use view space light positions, in
// whatever space excpet view
vec4 blinn(vec3 vertex, vec3 normal, vec3 camera, light_source lights[LIGHT_COUNT]){
  // will this code be optimized by opengl ? only light position is from uniform
  light_product product = get_light_product(vertex, normal, camera, mtl, lights);
  vec4 color = mtl.emission + lm.ambient * mtl.ambient +
    product.ambient + product.diffuse + product.specular;
  return color;
}
