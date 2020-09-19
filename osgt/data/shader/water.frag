#version 120

varying vec3 normal;
varying vec3 vertex;

void main()
{
    vec3 n = normalize(normal);
    const vec3 light_dir = normalize(vec3(-1, 1, 0.5));
    float ndotl = clamp(dot(n, light_dir), 0, 1);

    vec3 color = vec3(1) * ndotl;

    if (ndotl > 0)
    {
        vec3 h = normalize(light_dir + normalize(vec3(0, 0, 1)));
        float ndoth = clamp(dot(n, h), 0, 1);

        color += vec3(1) * pow(ndoth, 27.);
    }

    gl_FragColor = vec4(color, 1);
}
