#version 300 es

#ifdef GL_ES
precision highp float;
#endif

in vec2 fragTexCoord;

struct BsMat{
    sampler2D diffTex;
    sampler2D specTex;
    float shine;
    vec3 clr;
};

uniform BsMat uMat;

out vec4 fragClr;

void main(){
    fragClr = texture(uMat.diffTex, fragTexCoord);
    fragClr += 0.2*vec4(uMat.clr, 1.0);
}
