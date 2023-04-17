#version 430 core

out vec4 fColor;


in vec4 vNormal;
in vec4 vPosition;
in vec2 vTextureCoord;
in vec4 vColor;

uniform layout(location = 4) sampler2D sampler;
uniform layout(location = 5) sampler2D sampler2;
uniform layout(location = 6) int surface_effect;

// light
uniform layout(location = 7) vec4 ambient_light;
uniform layout(location = 8) vec4 specular_light;
uniform layout(location = 9) vec4 diffuse_light;

// material
uniform layout(location = 10) vec4 ambient_color;
uniform layout(location = 11) vec4 specular_color;
uniform layout(location = 12) vec4 diffuse_color;
uniform layout(location = 13) float specular_shininess;     // exponent for sharpening highlights
uniform layout(location = 14) float specular_strength;      // extra factor to adjust shininess

uniform layout(location = 15) vec4 light_position; // position of the light
uniform layout(location = 16) vec3 camera_position;







void main()
{
        vec4 base_color;
        switch(surface_effect) {
                case 0:
                        base_color = vColor;
                        break;
                case 1:
                        base_color = diffuse_color;
                        break;
                case 2:
                        base_color = texture(sampler, vTextureCoord);
                        break;
                default:
                        base_color = vec4(1,0,1,1);
                        break;
      }
        fColor = base_color;

        vec3 light_direction = vPosition.xyz - light_position.xyz;
//        vec3 light_direction = light_position.xyz;
        vec3 vnn = normalize(vNormal.xyz);
        float diffuse_factor = max(0.0, dot(vnn, -normalize(light_direction)));
        vec3 half_vector = normalize(normalize(-light_direction) - normalize(vPosition.xyz-camera_position.xyz));
        float specular_factor = max(0.0, dot(vnn, half_vector));
        if (diffuse_factor == 0.0)
            specular_factor = 0.0;
        else
            specular_factor = pow(specular_factor, specular_shininess) * specular_strength;  // sharpen the highlight
        vec4 texture_sample = texture(sampler, vTextureCoord);
        vec4 ambient_component = ambient_color * ambient_light;
        vec4 diffuse_component = diffuse_factor * base_color * diffuse_light;
        vec4 specular_component = specular_factor * specular_color * specular_light;
        vec4 total = ambient_component + diffuse_component + specular_component;
        fColor = vec4(total.rgb, 1.0);

}
