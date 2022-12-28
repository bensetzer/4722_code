#version 430 core

precision highp float;

vec2 ctimes(vec2 a, vec2 b) {
    return vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x);
}

vec2 crecip(vec2 a) {
    float n = a.x*a.x + a.y*a.y;
    return vec2(a.x/n, -a.y/n);
}

vec2 csquare(vec2 a) {
    return ctimes(a,a);
}

vec2 ccube(vec2 a) {
    return ctimes(csquare(a),a);
}

vec2 iterator(vec2 z) {
    vec2 num = 2.0 * ccube(z) + vec2(1.0,0.0);
    vec2 denr = crecip(3.0 * csquare(z));
    return ctimes(num, denr); 
}

in vec2 vPos;

uniform int num_colors;
uniform vec4 colors[20] ;
const float eps = .00001;
uniform int iteration_limit;

const vec2 root0 = vec2(1.0, 0.0);
const vec2 root1 = vec2(-0.5, sqrt(3.0)/2.0);
const vec2 root2 = vec2(-0.5, -sqrt(3.0)/2.0);

out vec4 fColor;

void
main()
{


    vec4 cl = vec4(0.0,0.0,1.0,0.0);
    vec2 z = vPos;


    vec4 color = vec4(0,0,0,1);

    int i;
    for(i = 0; i < iteration_limit; i++ ) {
        if(distance(z,root0) < eps) {
            color = colors[0];
            break;
        } else if(distance(z,root1) < eps) {
            color = colors[4];
            break;
        } else if(distance(z,root2) < eps) {
            color = colors[8];
            break;
        } else {
            z = iterator(z);
        }
    }

    fColor = color;
    //fColor = (i < iteration_limit) ? colors[i % num_colors] : vec4(0,0,0,1);
    
}