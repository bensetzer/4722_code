#version 430 core

/**
In this implementation, the shading calculations are done on a fragment by fragment basis.
This tends to give better results than computing vertex colors and interpolating (Gouraud shading).

There are three components computed: ambient, diffuse, and specular.

The text book and the notes describe how the calculations approximate actual physical lighting effects.
*/


out vec4 fColor;


in vec4 wNormal;
in vec4 wPosition;

/**
Light attributes and material attributes passed as uniform values.
*/
// light
uniform layout(location = 4) vec4 ambient_light;
uniform layout(location = 5) vec4 specular_light;
uniform layout(location = 6) vec4 diffuse_light;
uniform layout(location = 7) vec4 light_direction; // position of the light
uniform layout(location = 8) vec4 camera_position;

// material
uniform layout(location = 9) vec4 ambient_color;
uniform layout(location = 10) vec4 specular_color;
uniform layout(location = 11) vec4 diffuse_color;
uniform layout(location = 12) float specular_shininess;     // exponent for sharpening highlights
uniform layout(location = 13) float specular_strength;      // extra factor to adjust shininess






void main()
{

    // The normal vector is normalized to unit length.
    // This is necessary because we want to get the cosine of the angle the normal makes
    //      with other vectors.
    // We can use the dot product of unit vectors to get the cosine of the angle between them.
    vec3 vnn = normalize(wNormal.xyz);

    // the diffuse factor is the cosine of the angle between the normal and the vector
    //   from the fragment to the light.
    //   light_direction points the opposite way, so we negate it.
    // notice that we normalize the light_direction to get a unit vector
    //
    // The max function ensures that a negative dot product will be replaced by 0.
    //      That happens when the light is coming from behind the surface.
    //      In that case we don't want any diffuse lighting.
    //
    //  The diffuse factor is the most important part of the three-dimensional effect of shading.
    float diffuse_factor = max(0.0, dot(vnn, -normalize(light_direction.xyz)));


    /**
        The specular factor is computed similarly to the diffuse factor.
        The specular factor is the cosine of the angle between the normal and the half vector.

        The half vector is the average of the vector from the fragment to the light and the
            vector from the fragment to the camera.
    */
    vec3 half_vector = normalize(normalize(-light_direction.xyz) - normalize(wPosition.xyz-camera_position.xyz));
    float specular_factor = max(0.0, dot(vnn, half_vector));
    /**
       If the light is coming from behind the surface, there should be no specular lighting.
       This is not detected completely with the dot product just computed.
       So, we use the diffuse factor here: if the diffuse factor is 0, there should be no
        specular lighting.
    */
    if (diffuse_factor == 0.0)
    specular_factor = 0.0;
    else
    /**
        The final specular computation determines how wide the specular highlight should be.
        This is taken into account by raising the specular factor computed so far to the
            power of the specular shininess.
        The shinier the surface, the smaller the specular highlight.  A less shiny surface
            will blur the reflected light.
        Multiplying by the specular strength gives a way of brightening the specular highlight,
            making it more prominent.
    */
    specular_factor = pow(specular_factor, specular_shininess) * specular_strength;  // sharpen the highlight


    /**
        The final fragment color is computed in three parts: ambient, diffuse, specular.
        The base is the product of the material color and the light color in each case.
        For diffuse and specular, this product is multiplied by the corresponding factor.
            This is how shading is implented.
        The ambient component is the same no matter what the geometry, so there is no
            corresponding factor.
    */
    vec4 ambient_component = ambient_color * ambient_light;
    vec4 diffuse_component = diffuse_factor * diffuse_color * diffuse_light;
    vec4 specular_component = specular_factor * specular_color * specular_light;


    /**
        Add up the components to get the total color.
        Some parts (r or g or b) may add up to more than 1.
            In that case they are treated as 1.
    */
    vec4 total = ambient_component + diffuse_component + specular_component;


    /**
        Using vec4 here makes sure that the alpha is 1.
        This ensures that the color is opaque.
    */
    fColor = vec4(total.rgb, 1.0);

    //    fColor = vec4(vnn.xyz, 1.0);



}
