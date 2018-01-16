// 0 : occlusion
// 1 : steep
// 2 : with offset
// 3 : without offset
#ifndef PARALLAX_METHOD
#define PARALLAX_METHOD 0
#endif

#if PARALLAX_METHOD == 0
#define parallaxMap parallaxOcclusionMap
#elif PARALLAX_METHOD == 1
#define parallaxMap parallaxSteepMap
#elif PARALLAX_METHOD == 2
#define parallaxMap parallaxMapWithOffset
#elif PARALLAX_METHOD == 3
#define parallaxMap parallaxMapWithoutOffset
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

in VS_OUT{
  vec3 tangentVertex;
  vec3 tangentCamera;
  vec2 texcoord;
  LightSource tangentLights[LIGHT_COUNT];
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float heightScale = 1.0f;

out vec4 fragColor;

// without offset limiting
vec2 parallaxMapWithoutOffset(vec2 texcoord, vec3 viewDir){
  float height = texture(depthMap, texcoord).r;
  return texcoord - viewDir.xy * height * heightScale;
}

// with offset limiting
vec2 parallaxMapWithOffset(vec2 texcoord, vec3 viewDir){
  float height = texture(depthMap, texcoord).r;
  return texcoord - viewDir.xy/viewDir.z * height * heightScale;
}

// the 1st depth layter after the intersection point of -viewdir and
// geometry(from height map) has depth value greater than depth value in depth
// map
vec2 parallaxSteepMap(vec2 texcoord, vec3 viewDir){
  // 0,0,1 is normal in tangent space, if you look right at this surface, this
  // will be the viewDir, you won't need much layer in this case.
  float numLayers = mix(MAX_LAYERS, MIN_LAYERS, abs(dot(vec3(0.0,0.0,1.0), viewDir)));

	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * heightScale; 
	vec2 deltaTexcoord = P / numLayers;

  vec2  currentTexcoord = texcoord;
  float currentDepthMapValue = texture(depthMap, currentTexcoord).r;
  
  while(currentLayerDepth < currentDepthMapValue)
  {
    // shift texture coordinates along direction of P
    currentTexcoord -= deltaTexcoord;
    // get depthmap value at current texture coordinates
    currentDepthMapValue = texture(depthMap, currentTexcoord).r;  
    // get depth of next layer
    currentLayerDepth += layerDepth;  
  }

  return currentTexcoord;
}

vec2 parallaxOcclusionMap(vec2 texcoord, vec3 viewDir){
  // 0,0,1 is normal in tangent space, if you look right at this surface, this
  // will be the viewDir, you won't need much layer in this case.
  float numLayers = mix(MAX_LAYERS, MIN_LAYERS, abs(dot(vec3(0,0,1), viewDir)));

	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * heightScale; 
	vec2 deltaTexcoord = P / numLayers;

  vec2  currentTexcoord = texcoord;
  float currentDepthMapValue = texture(depthMap, currentTexcoord).r;
  
  while(currentLayerDepth < currentDepthMapValue)
  {
    // shift texture coordinates along direction of P
    currentTexcoord -= deltaTexcoord;
    // get depthmap value at current texture coordinates
    currentDepthMapValue = texture(depthMap, currentTexcoord).r;  
    // get depth of next layer
    currentLayerDepth += layerDepth;  
  }

  vec2 previousTexcoord = currentTexcoord + deltaTexcoord;
  float d0 = currentDepthMapValue - currentLayerDepth;
  float d1 = (layerDepth - currentLayerDepth) - texture(depthMap,
      previousTexcoord).r;

  float weight = d0 / (d0 + d1);

  texcoord = currentTexcoord * (1-weight) + previousTexcoord * (weight);
  return texcoord;
}

void main(void)
{
  vec2 texcoord = parallaxMap(fs_in.texcoord, normalize(fs_in.tangentCamera -
        fs_in.tangentVertex));

  if(texcoord.x > 1.0 ||texcoord.y > 1.0 ||texcoord.x < 0.0 ||texcoord.y < 0.0 )
    discard;

  vec3 normal = texture(normalMap, texcoord).xyz * 2 - 1; 

  //LightProduct product = getLightProduct(fs_in.tangentVertex, normal,
  //fs_in.tangentCamera, material, fs_in.tangentLights);
  //vec4 diffuse = material.emission + lightModel.ambient * material.ambient +
  //product.ambient + product.diffuse;

  //diffuse *= texture(diffuseMap, fs_in.texcoord);
  //fragColor = diffuse + product.specular;
  //fragColor.a = product.diffuse.a;

  fragColor = blinn(fs_in.tangentVertex, normal, fs_in.tangentCamera, fs_in.tangentLights);
  fragColor *= texture(diffuseMap, fs_in.texcoord);
}
