#version 430 core

out vec4 fColor;

//in vec4 vNormal;
in vec4 vPosition;

uniform layout(location = 6) vec4 ambient_product;
uniform layout(location = 7) vec4 specular_product;
uniform layout(location = 8) vec4 diffuse_product;

uniform layout(location = 5) vec4 light_position;  // in view coordinates
uniform layout(location = 9) float specular_shininess;     // exponent for sharpening highlights
uniform layout(location = 10) float specular_strength;      // extra factor to adjust shininess

uniform layout(location = 11) vec4 SurfaceColor = vec4(0.7, 0.6, 0.18, 1.0);
uniform layout(location = 12) float BumpDensity = 5.0;
uniform layout(location = 13) float BumpSize = 0.15;
uniform layout(location = 14) float SpecularFactor = 0.5;


in vec3 LightDir;
in vec3 EyeDir;
in vec2 TexCoord;

void main()
{
    vec3 litColor;
    vec2 c = BumpDensity * TexCoord.st;
    vec2 p = fract(c) - vec2(0.5);
    float d, f; d = dot(p,p);
    f = inversesqrt(d + 1.0);
    if (d >= BumpSize) {
        p = vec2(0.0);
        f = 1.0;
    }
    vec3 normDelta = vec3(p.x, p.y, 1.0) * f;

    vec3 light_direction = LightDir;
    //vec4 light_direction = vPosition - light_position;
    vec3 half_vector = normalize(normalize(-light_direction.xyz) - normalize(vPosition.xyz));
    
    vec3 vnn = normalize(normDelta);

    float diffuse_factor = max(0.0, dot(vnn, -normalize(light_direction)));

    vec3 reflectDir = reflect(LightDir, normDelta);
    float specular_factor = max(dot(EyeDir, reflectDir), 0.0);
//    float specular_factor = max(0.0, dot(vnn.xyz, half_vector));

    if (diffuse_factor == 0.0)
        specular_factor = 0.0;
    else
       specular_factor = pow(specular_factor, specular_shininess) * specular_strength;  // sharpen the highlight


    vec4 ambient_component = ambient_product;
    vec4 diffuse_component = diffuse_factor * diffuse_product;
    vec4 specular_component = specular_factor * specular_product;

    vec4 total = ambient_component + diffuse_component + specular_component;

    fColor = vec4(total.rgb, 1.0);
  
}
