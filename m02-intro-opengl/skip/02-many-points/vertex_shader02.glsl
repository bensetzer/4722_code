
#version 330 core

/*
    Each time the vertex shader is executed the next element in the buffer associated
    with vPosition will be assigned to vPosition.

    The 'in' modifier used in the vertex shader indicates that the variable receives its value
        from a buffer.
*/
in vec4 vPosition;

/*
    The draw operation specifies how many times the vertex shader is executed.
    That means that the function main is called that many times.
    Each time the function main executes data from a new vertex is assigned to the 'in'
        variable.
*/
void main()
{
    /*
        The position of this vertex is used unchanged.
    */
    gl_Position = vPosition;
}
