#version 300 es
#ifdef GL_ES
precision highp float;
#endif
//in vec2 text;
//in vec3 norm;
uniform vec4 lightColor;
uniform vec4 objectColor;
out vec4 fragColor;
void main() {
    fragColor = lightColor * objectColor;
};
