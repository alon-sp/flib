#version 300 es

in vec3 aRectVertexPos;
in vec3 aRectVertexColor;

out vec3 vRectVertexColor;

void main(){
    gl_Position = vec4(aRectVertexPos, 1.0f);
    vRectVertexColor = aRectVertexColor;
}
