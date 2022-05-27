#version 330 core

in vec3 vtxPos;
in vec3 vtxClr;
in vec3 vtxNorm;
in vec2 vtxTexCoord;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 fragClr;
out vec3 fragNorm;
out vec3 fragPos;
out vec2 fragTexCoord;

void main(){
    gl_Position = uProj*uView*uModel * vec4(vtxPos, 1.0f);
    
    fragPos = vec3( uView*uModel*vec4(vtxPos, 1.0f) );
    fragNorm = vec3( uView*uModel*vec4(vtxNorm, 0.0f) );
    fragClr = vtxClr;
    fragTexCoord = vtxTexCoord;
}
