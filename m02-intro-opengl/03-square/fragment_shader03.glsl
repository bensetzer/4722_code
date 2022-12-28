#version 430 core

/*
 * This variable receives a value from the vColor variable in the vertex shader.
 * However, there is some complexity if there are multiple vertices for a primitive (line or triangle)
 *
 * If the primitives are points, then each point is described by a single vertex.
 * The color for that vertex, assigned to vColor in the vertex shader, is received in the vColor variable
 *      defined below and then used as the fragment color.
 *
 *  If, however, several vertices are used to describe a primitive, then the situation is more difficult.
 *      Three vertices are used to describe a triangle.
 *      There are potentially three different colors used for the three vertices.
 *      The color assigned to vColor in that case is a weighted average of the three colors
 *          based on how far the fragment is from each vertex.
 *          The closer a fragment is to a particular, the more of that color is used
 *          in the vColor value.
*       This process is called 'interpolation'.
 *      Interpolation will happen whenever values are sent from vertex shader to fragment shader.
 */
in vec4 vColor;


out vec4 fColor;

void main()
{
    fColor = vColor;
}
