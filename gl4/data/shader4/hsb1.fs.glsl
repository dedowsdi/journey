#version 430 core

#define PI 3.14159265359
#define TWO_PI 6.283185

uniform vec2 resolution;

vec3 hsb2rgb( in vec3 c ){
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                             6.0)-3.0)-1.0, 0.0, 1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix( vec3(1.0), rgb, c.y);
}

void main(){
    vec2 st = gl_FragCoord.xy/resolution.xy;
    vec2 ray = st - vec2(0.5);
    float angle = atan(ray.y, ray.x);
    float saturation = length(ray) * 2.0;
    if(saturation > 1)
      discard;
    float hue = angle/TWO_PI;
    // if you want to avoid this if, you can use -ray to calculate angle, then
    // set hue = angle/TWO_PI + 0.5,  +0.5 works as add PI degree to ray
    if(hue < 0)
      hue += 1;

    vec3 color = hsb2rgb(vec3(hue, saturation, 1.0));

    gl_FragColor = vec4(color,1.0);
}
