#version 330

#ifdef GL_ES
precision highp float;
#endif

//in vec2 text;
in vec3 norm;
in vec3 pos;

uniform vec4 ambientLight;
uniform vec4 objectColor;
uniform vec3 sunDirection;
uniform vec4 sunLightColor;

out vec4 fragColor;

void main() {
	vec3 lightDir = normalize(sunDirection - pos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = diff * sunLightColor;
    fragColor = (ambientLight + diffuse) * objectColor;
}
