#version 120
#pragma include shader/ntoy.frag

// Sphere mapping by nimitz (twitter: @stormoid)

/*
        Little demo of different sphere uv mapping techniques.
*/

// #define time iTime
// #define time osg_SimulationTime
#define time 0

mat2 mm2(in float a)
{
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

vec3 hsv2rgb(in vec3 c)
{
    vec3 rgb = clamp(
        abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
    return c.z * mix(vec3(1.0), rgb, c.y);
}

vec3 tex(in vec2 p)
{
    float frq = 50.3;
    p += 0.405;
    return vec3(1.) *
           smoothstep(.9, 1.05, max(sin((p.x) * frq), sin((p.y) * frq)));
}

// Cube projection, cheap to compute and not too much deformation
vec3 cubeproj(in vec3 p)
{
    vec3 x = tex(p.zy / p.x);
    vec3 y = tex(p.xz / p.y);
    vec3 z = tex(p.xy / p.z);

    // simple coloring/shading
    x *= vec3(1, 0, 0) * abs(p.x) + p.x * vec3(0, 1, 0);
    y *= vec3(0, 1, 0) * abs(p.y) + p.y * vec3(0, 0, 1);
    z *= vec3(0, 0, 1) * abs(p.z) + p.z * vec3(1, 0, 0);

    // select face
    p = abs(p);
    if (p.x > p.y && p.x > p.z)
        return x;
    else if (p.y > p.x && p.y > p.z)
        return y;
    else
        return z;
}

// HEALPix  (debugged and cleaned from:
// http://www.cse.cuhk.edu.hk/~ttwong/papers/spheremap/spheremap.html) Has
// several advantages, might not be as versatile as cubemapped.
vec3 healpix(vec3 p)
{
    float a = atan(p.z, p.x) * 0.63662;
    float h = 3. * abs(p.y);
    float h2 = .75 * p.y;
    vec2 uv = vec2(a + h2, a - h2);
    h2 = sqrt(3. - h);
    float a2 = h2 * fract(a);
    uv = mix(uv, vec2(-h2 + a2, a2), step(2., h));

    vec3 col = tex(uv);
    col.x = a * 0.5;
    return hsv2rgb(vec3(col.x, .8, col.z));
}

// Triplanar, not really a sphere mapping, but adding it for completeness sake
vec3 tpl(in vec3 p)
{
    vec3 x = tex(p.yz);
    vec3 y = tex(p.zx);
    vec3 z = tex(p.xy);

    // simple coloring/shading
    x *= vec3(1, 0, 0) * abs(p.x) + p.x * vec3(0, 1, 0);
    y *= vec3(0, 1, 0) * abs(p.y) + p.y * vec3(0, 0, 1);
    z *= vec3(0, 0, 1) * abs(p.z) + p.z * vec3(1, 0, 0);

    p = normalize(max(vec3(0), abs(p) - .6));
    return x * p.x + y * p.y + z * p.z;
}

// Spherical coordinates, very heavy deformation on the poles
vec3 sphproj(in vec3 p)
{
    vec2 sph = vec2(acos(p.y / length(p)), atan(p.z, p.x));

    vec3 col = tex(sph * .9);
    col.x = sph.x * 0.4;
    return hsv2rgb(vec3(col.x, .8, col.z));
}

float iSphere(in vec3 ro, in vec3 rd)
{
    vec3 oc = ro;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - 1.;
    float h = b * b - c;
    if (h < 0.0) return -1.;
    return -b - sqrt(h);
}

void main()
{
    vec2 p = gl_FragCoord.xy / resolution.xy - 0.5;
    p.x *= resolution.x / resolution.y;
    vec2 um = mouse.xy / resolution.xy - .5;
    um.x *= resolution.x / resolution.y;
    p *= 4;

    // camera
    vec3 ro = vec3(0., 0., 2.4);
    vec3 rd = normalize(vec3(p, -1.5));
    mat2 mx = mm2(time * .4 + um.x * 5.);
    mat2 my = mm2(time * 0.3 + um.y * 5.);
    ro.xz *= mx;
    rd.xz *= mx;
    ro.xy *= my;
    rd.xy *= my;

    float sel = mod(floor(time * 0.3), 4.);

    float t = iSphere(ro, rd);
    vec3 col = vec3(0);
    if (sel == 0.)
        col = cubeproj(rd) * 1.1;
    else if (sel == 1.)
        col = tpl(rd) * 1.2;
    else if (sel == 2.)
        col = healpix(rd);
    else
        col = sphproj(rd);

    if (t > 0.)
    {
        vec3 pos = ro + rd * t;
        if (sel == 0.)
            col = cubeproj(pos) * 1.1;
        else if (sel == 1.)
            col = tpl(pos) * 1.2;
        else if (sel == 2.)
            col = healpix(pos);
        else
            col = sphproj(pos);
    }

    gl_FragColor = vec4(col, 1.0);
}
