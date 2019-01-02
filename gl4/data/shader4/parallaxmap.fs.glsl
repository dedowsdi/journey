// 0 : occlusion
// 1 : steep
// 2 : with offset
// 3 : without offset
#ifndef PARALLAX_METHOD
#define PARALLAX_METHOD 0
#endif

#if PARALLAX_METHOD == 0
#define parallax_map parallax_occlusion_map
#elif PARALLAX_METHOD == 1
#define parallax_map parallax_steep_map
#elif PARALLAX_METHOD == 2
#define parallax_map parallax_map_with_offset
#elif PARALLAX_METHOD == 3
#define parallax_map parallax_map_without_offset
#endif

#ifndef LIGHT_COUNT
#define LIGHT_COUNT
#endif

#ifndef MIN_LAYERS
#define MIN_LAYERS 4.0
#endif

#ifndef MAX_LAYERS
#define MAX_LAYERS 16.0
#endif

in vs_out{
  vec3 t_vertex;
  vec3 t_camera;
  vec2 texcoord;
  light_source t_lights[LIGHT_COUNT];
} fi;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D depth_map;
uniform float height_scale = 1.0f;

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];

out vec4 frag_color;

// without offset limiting
vec2 parallax_map_without_offset(vec2 texcoord, vec3 v_dir){
  float height = texture(depth_map, texcoord).r;
  return texcoord - v_dir.xy * height * height_scale;
}

// with offset limiting
vec2 parallax_map_with_offset(vec2 texcoord, vec3 v_dir){
  float height = texture(depth_map, texcoord).r;
  return texcoord - v_dir.xy/v_dir.z * height * height_scale;
}

// the 1st depth layter after the intersection point of -viewdir and
// geometry(from height map) has depth value greater than depth value in depth
// map
vec2 parallax_steep_map(vec2 texcoord, vec3 v_dir){
  // 0,0,1 is normal in tangent space, if you look right at this surface, this
  // will be the v_dir, you won't need much layer in this case.
  float num_layers = mix(MAX_LAYERS, MIN_LAYERS, abs(dot(vec3(0.0,0.0,1.0), v_dir)));

	float layer_depth = 1.0 / num_layers;
	float current_layer_depth = 0.0;

	// the amount to shift the texture coordinates per layer (from vector p)
	vec2 p = v_dir.xy * height_scale; 
	vec2 delta_texcoord = p / num_layers;

  vec2  current_texcoord = texcoord;
  float current_depth_map_value = texture(depth_map, current_texcoord).r;
  
  while(current_layer_depth < current_depth_map_value)
  {
    // shift texture coordinates along direction of p
    current_texcoord -= delta_texcoord;
    // get depthmap value at current texture coordinates
    current_depth_map_value = texture(depth_map, current_texcoord).r;  
    // get depth of next layer
    current_layer_depth += layer_depth;  
  }

  return current_texcoord;
}

vec2 parallax_occlusion_map(vec2 texcoord, vec3 v_dir){
  // 0,0,1 is normal in tangent space, if you look right at this surface, this
  // will be the v_dir, you won't need much layer in this case.
  float num_layers = mix(MAX_LAYERS, MIN_LAYERS, abs(dot(vec3(0,0,1), v_dir)));

	float layer_depth = 1.0 / num_layers;
	float current_layer_depth = 0.0;

	// the amount to shift the texture coordinates per layer (from vector p)
	vec2 p = v_dir.xy * height_scale; 
	vec2 delta_texcoord = p / num_layers;

  vec2  current_texcoord = texcoord;
  float current_depth_map_value = texture(depth_map, current_texcoord).r;
  
  while(current_layer_depth < current_depth_map_value)
  {
    // shift texture coordinates along direction of p
    current_texcoord -= delta_texcoord;
    // get depthmap value at current texture coordinates
    current_depth_map_value = texture(depth_map, current_texcoord).r;  
    // get depth of next layer
    current_layer_depth += layer_depth;  
  }

  vec2 previous_texcoord = current_texcoord + delta_texcoord;
  float d0 = current_depth_map_value - current_layer_depth;
  float d1 = (layer_depth - current_layer_depth) - texture(depth_map,
      previous_texcoord).r;

  float weight = d0 / (d0 + d1);

  texcoord = current_texcoord * (1-weight) + previous_texcoord * (weight);
  return texcoord;
}

void main(void)
{
  vec2 texcoord = parallax_map(fi.texcoord, normalize(fi.t_camera -
        fi.t_vertex));

  if(texcoord.x > 1.0 ||texcoord.y > 1.0 ||texcoord.x < 0.0 ||texcoord.y < 0.0 )
    discard;

  vec3 normal = texture(normal_map, texcoord).xyz * 2 - 1; 

  //light_product product = get_light_product(fi.t_vertex, normal,
  //fi.t_camera, material, fi.tangentLights);
  //vec4 diffuse = material.emission + light_model.ambient * material.ambient +
  //product.ambient + product.diffuse;

  //diffuse *= texture(diffuse_map, fi.texcoord);
  //frag_color = diffuse + product.specular;
  //frag_color.a = product.diffuse.a;

  frag_color = blinn(fi.t_vertex, normal, fi.t_camera, mtl, fi.t_lights, lm);
  frag_color *= texture(diffuse_map, fi.texcoord);
}
