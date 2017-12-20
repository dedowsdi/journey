#version 120
#extension GL_EXT_gpu_shader4 : require

varying vec3 vertex;  // local
varying vec3 normal;  // local

uniform vec3 eye;       // model space
uniform vec3 lightDir;  // to light, normalized
uniform sampler2D depthMap;
//sample the shadowmap once, the hardware will in fact also sample the neighboring
//texels, do the comparison for all of them, and return a float in [0,1] with a
//bilinear filtering of the comparison results.
uniform sampler2DShadow shadowMap;
uniform float bias = 0.005;
uniform bool useSampler2DShadow = true;

void main(void) {
  // normal blinn-phone
  vec3 l = lightDir;
  vec3 n = normalize(normal);
  vec3 v = normalize(eye - vertex);
  vec3 h = normalize(l + v);
  float ndotl = clamp(dot(n, l), 0.0, 1.0);

  float visibility = 1.0;

  /*
   * As depth map is discrete, different fragments with different depth might
   * sample the same texel, this could cause shadow acne, bias is used to
   * eliminite shaodw acne.
   */
  float shadowBias = clamp(bias * tan(acos(ndotl)), 0, 0.1);

  // uncomment this to unshadow area beyound far or near plane of light proj
  // if (gl_TexCoord[1].z < 1 && gl_TexCoord[1].z > 0) {
  // positive only if it's really "far" from the depth texel
  // if (gl_TexCoord[1].z -shadowBias > texture2D(depthMap,
  // gl_TexCoord[1].xy).z) {
  //if (gl_TexCoord[1].z - shadowBias >
      //shadow2DProj(depthMap, vec4(gl_TexCoord[1])).r) {
    //visibility = 0.5;
  //}
  //}
  if(useSampler2DShadow)
  {
    visibility *= shadow2DProj(shadowMap, 
        vec4(gl_TexCoord[1].xy, gl_TexCoord[1].z - shadowBias, gl_TexCoord[1].w)).z;
  }
  else
  {
    //if (gl_TexCoord[1].z - shadowBias > texture2D(depthMap, vec4(gl_TexCoord[1]).xy).z) {
      //visibility = 0.5;
    //}
    visibility = 0.0;
    vec2 texelSize = 1.0 / textureSize2D(depthMap, 0);
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; ++j) {
        float pcfDepth = texture2D(depthMap, vec4(gl_TexCoord[1]).xy +texelSize
            * vec2(i, j) ).z;
        visibility += (gl_TexCoord[1].z - shadowBias > pcfDepth) ? 0 : 1;
      }
    }
    visibility /= 9.0;
  }
  
  vec4 diffuse = ndotl * gl_LightSource[0].diffuse;
  vec4 specular = vec4(0.0);
  if (ndotl > 0.0) {
    float ndoth = clamp(dot(n, h), 0.0, 1.0);
    specular =
      pow(ndoth, gl_BackMaterial.shininess) * gl_LightSource[0].specular;
  }

  gl_FragColor = visibility * (diffuse + specular);
}
