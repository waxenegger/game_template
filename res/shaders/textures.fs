#version 330 core

in vec3 norm;
in vec3 pos;
in vec2 uvCoords;

uniform vec3 ambientLight;
uniform vec4 objectColor;
uniform vec3 sunDirection;
uniform vec3 sunLightColor;

uniform sampler2D tex0;

out vec4 fragColor;

void main() {
	vec3 lightDir = normalize(sunDirection - pos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * sunLightColor;
	vec4 color = normalize(vec4(ambientLight + diffuse, 1.0) * objectColor);
	fragColor = texture(tex0, uvCoords) * color;
}
