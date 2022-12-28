#version 430 core

//  output color for the fragment
out vec4 fColor;

// interpolated normal vector for this fragment
// this fragment is a pixel from a particular triangle
// The normals from the three vertices of that triangle are averaged
//      the average is weighted based on how far the fragment is from each vertex
// This gives a reasonably good approximation to the actual normal at this point on the surface
in vec4 vNormal;

void main()
{
    //  normalize the vec3 made from the rgb components
    // the resulting vec3 has components in the range from -1 to 1
    vec3 normed = normalize(vNormal.rgb);
    // take the absolute value of normed
    // the resulting vec3 has compoents in the range from 0 to 1
    vec3 anormed = abs(normed);
    // attach an alpha of 1.0 to anormed to make a vec4 and use that as the fragment color
    fColor = vec4(anormed, 1.0);
//    fColor = abs(vec4(normalize(vNormal.rgb), 1.0));
}
