#version 300 es

#ifdef GL_ES
precision highp float;
#endif

in vec3 fragPos;
in vec3 fragNorm;
in vec2 fragTexCoord;

struct BmMat{
    sampler2D diffTex;
    sampler2D specTex;
    float shine;
    vec3 clr;
};

uniform BmMat uMat;
uniform vec3 uLightDir;
uniform vec3 uLightClr;
uniform mat4 uModel;
uniform mat4 uView;

out vec4 fragCLR;

void main(){

    //Ambient Light
    vec3 lightAmb = 0.01*uLightClr;

    vec3 fNorm = normalize(fragNorm);
    vec3 lightDir = vec3(0, 0, 1);// normalize( vec3(uView*vec4(-uLightDir, 0)) );

    //diffuse light
    float diff = max(dot(fNorm, lightDir), 0.0f);
    vec3 lightDiff = diff*(0.5*uLightClr);

    //specular light: blinn phong
    vec3 h = normalize(-fragPos + lightDir);
    float spec = pow( max(dot(h, fNorm), 0.0f), uMat.shine );
    vec3 lightSpec = spec*uLightClr;

    //
    fragCLR = vec4( uMat.clr*(lightDiff + lightAmb + lightSpec), 1);
}
