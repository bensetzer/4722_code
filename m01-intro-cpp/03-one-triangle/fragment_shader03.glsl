#version 430 core


/*
    Output from the fragment shader can be complex.
    We will take the simplest path: output a single color for the fragment.
    Avoid defining multiple out variables in the fragment shader, the consequences can
        be unpredictable.
*/
out vec4 fColor;

/*
    We get the color to use for the fragment from the uniform variable.
    The value is set in the main program function 'render'.
    The value is set just before calling glDrawArrays which starts the pipeline.
*/
uniform layout(location = 1) vec4 color;

void main()
{
    // copy the input color to the output.
    fColor = color;
}
