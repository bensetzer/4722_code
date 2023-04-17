
#version 430 core

in layout(location = 1) vec4 bPosition;
in layout(location = 2) vec4 bColor;
in layout (location = 3) vec2 bTextureCoord;
in layout (location = 4) vec4 bNormal;


uniform layout(location = 1) mat4 m_transform;
uniform layout(location = 2) mat4 vp_transform;
uniform layout(location = 3) mat4 normal_transform;


out vec4 vNormal;
out vec4 vPosition;
out vec2 vTextureCoord;
out vec4 vColor;

void
main()
{
    vColor = bColor;
    vTextureCoord = bTextureCoord;
    vNormal = normal_transform * bNormal;
    vPosition = m_transform * bPosition;
    gl_Position =  vp_transform * vPosition;
}
