#version 330 core

in vec3 vtxPos;
in vec2 vtxTexCoord;

out vec2 fragTexCoord;

void main(){
    gl_Position = vec4(vtxPos, 1.0f);
    
    fragTexCoord = vtxTexCoord;
}
