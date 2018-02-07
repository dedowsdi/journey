varying vec3 t_vertex; 
varying vec3 t_camera;
varying vec2 m_texcoord;
varying vec4 t_light_positions[LIGHT_COUNT];
varying vec3 t_spot_directions[LIGHT_COUNT];

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];
uniform sampler2D normal_map;

void main(void)
{
  vec3 t_normal = normalize(texture2D(normal_map, m_texcoord).xyz * 2 - 1); 
  light_source t_lights[LIGHT_COUNT] = lights;
  for (int i = 0; i < LIGHT_COUNT; i++) {
    t_lights[i].position = t_light_positions[i];
    t_lights[i].spot_direction = t_spot_directions[i];
  }
  
  gl_FragColor = blinn(t_vertex, t_normal, t_camera, mtl, t_lights, lm);
}
