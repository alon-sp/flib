#version 330 core

in vec2 fragTexCoord;

struct BmMat{
    sampler2D diffTex;
    sampler2D specTex;
    float shine;
};

uniform BmMat uMat;
uniform vec3 uMeshClr;

out vec4 fragClr;

void main(){
    fragClr = texture(uMat.diffTex, fragTexCoord);
    fragClr += 0.2*vec4(uMeshClr, 1.0);
}
