
#version 330 core

in vec4 bPosition;
/*
 * The colors are put in a buffer.
 * So they are delivered to the vertex shader.
 * Note that bColor is a vec4 since the values provided to the vertex shader are
 *      normalized.
 */
in vec4 bColor;

/*
 * The fragment shader is actually going to need the color.
 * The vertex shader cannot effectively color the pixels making up an image, only
 *      the fragment shader can.
 * This out variable corresponds to an 'in' variable of the same name in the
 *      fragment shader.
 * The two variables make a communication channel from the vertex shader to the fragment
 *      shader.
 */
out vec4 vColor;

void
main()
{
    /*
     * The vertex color is assigned to the out variable vColor and so is sent to the
     * fragment shader.
     */
    vColor = bColor;
    gl_Position = bPosition;
}
