#version 120

// adapted from https://www.shadertoy.com/view/llt3R4

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;

uniform vec2 resolution;
uniform float osg_SimulationTime;

#define time osg_SimulationTime
#define PI 3.1415926535897932384626433832795

float intersectSDF(float distA, float distB)
{
    // dist must smaller than all of them
    return max(distA, distB);
}

float unionSDF(float distA, float distB)
{
    // dist can be smaller than one of them
    return min(distA, distB);
}

float differenceSDF(float distA, float distB)
{
    // A - B = A ^ B~
    // we want to dig B out of A, distB is negative inside B
    return max(distA, -distB);
}

/* Return a transformation matrix that will transform a ray from view space
 * to world coordinates, given the eye point, the camera target, and an up vector.
 *
 * This assumes that the center of the camera is aligned with the negative z axis in
 * view space when calculating the ray marching direction.
 */
mat4 lookAtInverse(vec3 eye, vec3 center, vec3 up)
{
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(vec4(s, 0.0), vec4(u, 0.0), vec4(-f, 0.0), vec4(0.0, 0.0, 0.0, 1));
}

/**
 * Signed distance function for a sphere centered at the origin with radius 1.0;
 */
float sphere_sdf(vec3 sample_point)
{
    return length(sample_point) - 1.0;
}

float box_sdf(vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

/**
 * Signed distance function describing the scene.
 *
 * Absolute value of the return value indicates the distance to the surface.
 * Sign indicates whether the point is inside or outside the surface,
 * negative indicating inside.
 */
float scene_sdf(vec3 sample_point)
{
    return intersectSDF(sphere_sdf(sample_point), box_sdf(sample_point, vec3(0.8)));

    // some times you want to scale sample_point, keep in mind that when you scale
    // sample_point, you also scale the distance, you must compensate it
    // float dist = sphere_sdf(sample_point / scale) * scale;
}

/**
 * Using the gradient of the SDF, estimate the normal on the surface at point p.
 */
vec3 estimate_normal(vec3 p)
{
    // all components is divided by 2 * EPSILON, it's ignored since we normalize result
    return normalize(vec3(
        scene_sdf(vec3(p.x + EPSILON, p.y, p.z)) - scene_sdf(vec3(p.x - EPSILON, p.y, p.z)),
        scene_sdf(vec3(p.x, p.y + EPSILON, p.z)) - scene_sdf(vec3(p.x, p.y - EPSILON, p.z)),
        scene_sdf(vec3(p.x, p.y, p.z + EPSILON)) -
            scene_sdf(vec3(p.x, p.y, p.z - EPSILON))));
}

/**
 * Return the normalized direction (view space) to march in from the eye point for a single
 * pixel.
 *
 * fieldOfView: vertical field of view in degrees
 * size: resolution of the output image
 * fragCoord: the x,y coordinate of the pixel in the output image
 */
vec3 ray_direction(float fieldOfView, vec2 size, vec2 fragCoord)
{
    vec2 xy = fragCoord - size / 2.0;
    // unlike scratchpixel, z part is not fixed to -1
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}

/**
 * Return the shortest distance from the eyepoint to the scene surface along
 * the marching direction. If no part of the surface is found between start and end,
 * return end.
 *
 * eye: the eye point, acting as the origin of the ray
 * dir: the normalized direction to march in
 * start: the starting distance away from the eye
 * end: the max distance away from the ey to march before giving up
 */
float shortest_distance_to_surface(vec3 eye, vec3 dir, float start, float end)
{
    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++)
    {
        float dist = scene_sdf(eye + depth * dir);
        if (dist < EPSILON)
        {
            return depth;
        }
        depth += dist;
        if (depth >= end)
        {
            return end;
        }
    }
    return end;
}

vec4 light(vec3 pos)
{
    vec3 normal = estimate_normal(pos);
    float t = osg_SimulationTime;
    vec3 L = normalize(vec3(cos(time), sin(time), 1));
    float ndotl = dot(normal, L);
    return vec4(ndotl);
}

void main(void)
{
    const vec3 eye = vec3(10);
    mat4 viewMatrixInverse = lookAtInverse(eye, vec3(0), vec3(0, 1, 0));

    vec3 dir = ray_direction(45.0, resolution.xy, gl_FragCoord.xy);
    dir = mat3(viewMatrixInverse) * dir;
    float dist = shortest_distance_to_surface(eye, dir, MIN_DIST, MAX_DIST);

    if (dist > MAX_DIST - EPSILON)
    {
        // Didn't hit anything
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    gl_FragColor = light(eye + dir * dist);
}
