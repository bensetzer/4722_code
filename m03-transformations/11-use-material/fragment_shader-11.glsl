#version 430 core

/*
 * The fragment shader gets the surface color in the variable 'diffuse_color'
 * and simply assigns that value as the fragment color.
 */

out vec4 fColor;


uniform layout(location = 2) vec4 diffuse_color;

void main()
{
    fColor = diffuse_color;
}
