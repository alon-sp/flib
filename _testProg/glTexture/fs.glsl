#version 330 core

in vec3 vClr;
in vec2 vTexCoord;

uniform sampler2D uTex1;
uniform sampler2D uTex2;

out vec4 fragClr;

void main(){
    fragClr = mix(texture(uTex1,vTexCoord), texture(uTex2, vTexCoord), 0.5);
    fragClr += 0.1*vec4(vClr, 1.0);
}
