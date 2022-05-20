#version 330 core

in vec3 aVtxPos;
in vec3 aVtxClr;
in vec2 aVtxTexCoord;

out vec3 vClr;
out vec2 vTexCoord;

void main(){
    gl_Position = vec4(aVtxPos, 1.0f);
    
    vClr = aVtxClr;
    vTexCoord = aVtxTexCoord;
}
