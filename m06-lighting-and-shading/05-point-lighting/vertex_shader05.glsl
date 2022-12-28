#version 430 core

in layout(location = 1) vec4 bPosition;
in layout(location = 2) vec4 bNormal;

uniform layout(location = 1) mat4 m_transform;
uniform layout(location = 2) mat4 vp_transform;
uniform layout(location = 3) mat4 normal_transform;


out vec4 wNormal;
out vec4 wPosition;


void
main()
{
    // The normal is transformed to world coordinates
    wNormal = normal_transform * bNormal;
    // The fragment position in the worldcoordinate frame is needed to compute shading.
    // So, the vertex position in world coordinates is sent to the fragment shader
    //      The corner positions will be interpolated to get the fragment position in world coordinates.
    wPosition = m_transform * bPosition;
    // Projecting the view coordinates finishes the transformation to the default coordinate frame.
    gl_Position =  vp_transform * wPosition;
}
