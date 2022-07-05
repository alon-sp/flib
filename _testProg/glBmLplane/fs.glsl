#version 330 core

in vec3 fragPos;
in vec3 fragNorm;
in vec2 fragTexCoord;

struct BsMat{
    sampler2D diffTex;
    sampler2D specTex;
    float shine;
    vec3 clr;
};

uniform BsMat uMat;
uniform vec3 uLightPos;
uniform vec3 uLightClr;
uniform mat4 uView;

out vec4 fragCLR;

void main(){

    //Ambient Light
    vec3 lightAmb = 0.1*uLightClr;

    vec3 fNorm = normalize(fragNorm);
    vec3 lightDir = normalize( vec3(uView*vec4(uLightPos, 1)) - fragPos );

    //diffuse light
    float diff = max(dot(fNorm, lightDir), 0);
    vec3 lightDiff = diff*(0.5*uLightClr);

    vec3 matDiff = vec3(texture(uMat.diffTex, fragTexCoord));

    //specular light: blinn phong
    vec3 h = normalize(-fragPos + lightDir);
    float spec = pow( max(dot(h, fNorm), 0), uMat.shine );
    vec3 lightSpec = spec*uLightClr;

    //
    fragCLR = vec4( matDiff*(lightDiff + lightAmb + lightSpec), 1);
}
