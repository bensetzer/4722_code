#version 430 core

out vec4 fColor;

in vec2 vTextureCoord;

uniform layout(location = 2) vec4 diffuse_color;
uniform layout(location = 3) float width;// = .6;
uniform layout(location = 4) float fuzz;// = .05;
uniform layout(location = 5) float scale;

void main()
{
    vec4 stripe_color = diffuse_color;
    vec4 back_color = vec4(vec3(1) - diffuse_color.rgb, 1);

    float coord = mix(vTextureCoord.t, vTextureCoord.s, 1);
//    float scaled_tc = fract(vTextureCoord.t * scale);
    float scaled_tc = fract(coord * scale);
    float frac1 = clamp(scaled_tc / fuzz, 0.0, 1.0);
    float frac2 = clamp((scaled_tc - width) / fuzz, 0.0, 1.0);
    float frac3 = frac1 * (1.0 - frac2);
    float frac4 = frac3 * frac3 * (3.0 - (2.0 * frac3));

    fColor =  mix(back_color, stripe_color, frac4);
}
