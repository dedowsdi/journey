// adapted from https://www.shadertoy.com/view/WslGWl

// ------------------------------------------
//
// This post cloned from "https://www.shadertoy.com/view/lss3zr"
//
// I also refer this blog post below.
// https://shaderbits.com/blog/creating-volumetric-ray-marcher
//
// This post is to learn how to cloud raymarching is working.
//
// ------------------------------------------

#define USE_LIGHT 0

uniform vec2 resolution;
uniform float osg_SimulationTime;
#define iTime osg_SimulationTime

// clang-format off
mat3 m = mat3(
        0.00, 0.80, 0.60,
        -0.80, 0.36, -0.48,
        -0.60, -0.48, 0.64
        );
// clang-format on

float hash(float n)
{
    return fract(sin(n) * 43758.5453);
}

///
/// Noise function
///
float noise(in vec3 x)
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f * f * (3.0 - 2.0 * f);

    float n = p.x + p.y * 57.0 + 113.0 * p.z;

    float res = mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
                        mix(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
        mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
            mix(hash(n + 170.0), hash(n + 171.0), f.x), f.y),
        f.z);
    return res;
}

///
/// Fractal Brownian motion.
///
/// Refer to:
/// EN: https://thebookofshaders.com/13/
/// JP: https://thebookofshaders.com/13/?lan=jp
///
float fbm(vec3 p)
{
    float f;
    f = 0.5000 * noise(p);
    p = m * p * 2.02;
    f += 0.2500 * noise(p);
    p = m * p * 2.03;
    f += 0.1250 * noise(p);
    return f;
}

//////////////////////////////////////////////////

///
/// Sphere distance function.
///
/// But this function return inverse value.
/// Normal dist function is like below.
///
/// return length(pos) - 0.1;
///
/// Because this function is used for density.
///
float scene(in vec3 pos)
{
    return 0.1 - length(pos) * 0.05 + fbm(pos * 0.3);
}

vec3 getNormal(in vec3 p)
{
    const float e = 0.01;
    return normalize(vec3(scene(vec3(p.x + e, p.y, p.z)) - scene(vec3(p.x - e, p.y, p.z)),
        scene(vec3(p.x, p.y + e, p.z)) - scene(vec3(p.x, p.y - e, p.z)),
        scene(vec3(p.x, p.y, p.z + e)) - scene(vec3(p.x, p.y, p.z - e))));
}

///
/// Create a camera pose control matrix.
///
mat3 camera(vec3 ray_origin, vec3 target)
{
    vec3 cw = normalize(target - ray_origin); // forward
    vec3 cp = vec3(0.0, 1.0, 0.0); // up
    vec3 cu = cross(cw, cp); // side
    vec3 cv = cross(cu, cw); // up
    return mat3(cu, cv, cw); // who is it not -cw?
}

///
/// Main function.
///
void main()
{
    vec2 uv = (gl_FragCoord.xy * 2.0 - resolution.xy) / min(resolution.x, resolution.y);

    vec2 mo = vec2(iTime * 0.1, cos(iTime * 0.25) * 3.0);

    // Camera
    float camDist = 25.0;

    // target
    vec3 target = vec3(0.0, 1.0, 0.0);

    // Ray origin
    // vec3 ori = vec3(sin(iTime) * camDist, 0, cos(iTime) * camDist);
    vec3 ro = camDist * normalize(vec3(cos(2.75 - 3.0 * mo.x), 0.7 - 1.0 * (mo.y - 1.0),
                            sin(2.75 - 3.0 * mo.x)));

    float targetDepth = 1.3;

    // Camera pose.
    mat3 c = camera(ro, target);
    vec3 dir = c * normalize(vec3(uv, targetDepth)); // not -targetDepth?

    // For raymarching const values.
    const int sampleCount = 64;
    const int sampleLightCount = 6;
    const float eps = 0.01;

    // Raymarching step settings.
    float zMax = 40.0;
    float zstep = zMax / float(sampleCount);

    float zMaxl = 20.0; // what's l?
    float zstepl = zMaxl / float(sampleLightCount);

    // Easy access to the ray origin
    vec3 p = ro;

    float transmittance = 1.0;

    // Substantially transparency parameter.
    float absorption = 100.0;
    vec3 sun_direction = normalize(vec3(1.0, 0.0, 0.0));

    // Result of culcration
    vec4 color = vec4(0.0);

    for (int i = 0; i < sampleCount; i++)
    {
        // Using distance function for density.
        // So the function not normal value.
        // Please check it out on the function comment.
        float density = scene(p);

        // The density over 0.0 then start cloud ray marching.
        // Why? because the function will return negative value normally.
        // But if ray is into the cloud, the function will return positive value.
        if (density > 0.0)
        {
            // why density sub by sampleCount?
            // This mean integral for each sampling points.
            float tmp = density / float(sampleCount);
            transmittance *= 1.0 - (tmp * absorption);

            if (transmittance <= 0.01)
                break;

#if USE_LIGHT == 1
            // Light scattering

            // Transmittance for Light
            float Tl = 1.0;

            // Start light scattering with raymarching.

            // Raymarching position for the light.
            vec3 lp = p;

            // Iteration of sampling light.
            for (int j = 0; j < sampleLightCount; j++)
            {
                float densityLight = scene(lp);

                // If densityLight is over 0.0, the ray is stil in the cloud.
                if (densityLight > 0.0)
                {
                    float tmpl = densityLight / float(sampleCount);
                    Tl *= 1.0 - (tmpl * absorption);
                }

                if (Tl <= 0.01)
                {
                    break;
                }

                // Step to next position.
                lp += sun_direction * zstepl;
            }
#endif

            // Add ambient + light scattering color
            float opaity = 50.;
            float k = opaity * tmp * transmittance;
            vec4 col1 = vec4(1.0) * k;

#if USE_LIGHT == 1
            float opacityl = 30.0;
            float kl = opacityl * tmp * transmittance * Tl;
            vec4 lightColor = vec4(1.0, 0.7, 0.9, 1.0);
            vec4 col2 = lightColor * kl;
#else
            vec4 col2 = vec4(0.0);
#endif

            color += col1 + col2;
        }

        p += dir * zstep;
    }

    vec3 bg = mix(vec3(0.3, 0.1, 0.8), vec3(0.7, 0.7, 1.0), 1.0 - (uv.y + 1.0) * 0.5);
    color.rgb += bg;

    gl_FragColor = color;
}
