#version 330 core

in vec3 norm;
in vec3 pos;

uniform vec3 ambientLight;
uniform vec4 objectColor;
uniform vec3 sunDirection;
uniform vec3 sunLightColor;

out vec4 fragColor;

void main() {
	vec3 lightDir = normalize(sunDirection - pos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * sunLightColor;
	fragColor = normalize(vec4(ambientLight + diffuse, 1.0) * objectColor);
}
