#version 120
#extension GL_EXT_gpu_shader4 : require

uniform vec3 eye;       // model space
uniform vec3 localLightPos;
uniform vec3 worldLightPos;
uniform samplerCube depthCubeMap;
uniform float bias = 0.05;
uniform float nearPlane; //light near
uniform float farPlane;  //light far

varying vec3 localVertex;
varying vec3 worldVertex;
varying vec3 normal;  // local


void main(void) {
  // normal blinn-phone
  vec3 l = normalize(localLightPos - localVertex);
  vec3 n = normalize(normal);
  vec3 v = normalize(eye - localVertex);
  vec3 h = normalize(l + v);
  float ndotl = clamp(dot(n, l), 0.0, 1.0);

  float visibility = 1.0;

  float shadowBias = clamp(bias * tan(acos(ndotl)), 0, 5);

  vec3 l2v = worldVertex - worldLightPos;
  float currentDepth = max(abs(l2v.x), max(abs(l2v.y), abs(l2v.z)));

  visibility = 0.0;
  float offset = 0.1;
  float numSamples = 4.0;
  //l2v = normalize(l2v);
  for (float x = -offset; x < offset; x+= 2 * offset/numSamples) {
    for (float y = -offset; y < offset; y+= 2 * offset/numSamples) {
      for (float z = -offset; z < offset; z+= 2 * offset/numSamples) {
        float pcfDepth = textureCube(depthCubeMap, l2v + vec3(x, y, z)).x;
        pcfDepth = pcfDepth * (farPlane - nearPlane) + nearPlane;
        visibility += (currentDepth - shadowBias > pcfDepth) ? 0 : 1;
      }
    }
  }
  visibility /= (numSamples * numSamples * numSamples);

  vec4 diffuse = ndotl * gl_LightSource[0].diffuse;
  vec4 specular = vec4(0.0);
  if (ndotl > 0.0) {
    float ndoth = clamp(dot(n, h), 0.0, 1.0);
    specular =
      pow(ndoth, gl_BackMaterial.shininess) * gl_LightSource[0].specular;
  }

  gl_FragColor = visibility * (diffuse + specular);
}
