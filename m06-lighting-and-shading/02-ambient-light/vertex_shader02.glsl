#version 430 core

in layout(location = 1) vec4 bPosition;

uniform layout(location = 1) mat4 transform;

void
main()
{
    gl_Position = transform * bPosition;
}
