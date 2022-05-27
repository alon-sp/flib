#version 330 core

in vec3 vClr;
in vec2 vTexCoord;

struct BmMat{
    sampler2D diffTex;
    sampler2D specTex;
    float shine;
};

uniform BmMat uMat;

out vec4 fragClr;

void main(){
    fragClr = texture(uMat.diffTex, vTexCoord);
    fragClr += 0.5*vec4(vClr, 1.0);
}
