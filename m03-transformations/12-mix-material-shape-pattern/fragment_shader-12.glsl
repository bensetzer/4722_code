#version 430 core

out vec4 fColor;

in vec4 vColor;

uniform layout(location = 2) vec4 diffuse_color;

void main()
{
    /*
     * The colors are mixed by multiplying them.
     * Multiplying by gray produces a lower intensity version of the other part of the product.
     */
    fColor = diffuse_color * vColor;
}
