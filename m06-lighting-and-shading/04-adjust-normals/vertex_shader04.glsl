#version 430 core

in layout(location = 1) vec4 bPosition;
in layout(location = 2) vec4 bNormal;

// what we have been calling 'transform' before is called 'mvp_transform' here
// this indicates that the transform is projection times view times animation times world
uniform layout(location = 1) mat4 mvp_transform;
// the normal transform computed in the driver program
uniform layout(location = 2) mat4 normal_transform;

out vec4 vNormal;


void
main()
{
    // transform the normal before sending to the fragment shader
    vNormal = normal_transform * bNormal;
    gl_Position =  mvp_transform * bPosition;
}
