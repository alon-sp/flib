#version 300 es

#ifdef GL_ES
precision highp float;
#endif

in vec3 vRectVertexColor;
out vec3 vertextColor;

void main(){
    vertextColor = vRectVertexColor;
}
