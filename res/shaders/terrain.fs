#version 330 core

in vec3 norm;
in vec3 pos;

uniform vec3 ambientLight;
uniform vec3 sunDirection;
uniform vec3 sunLightColor;
uniform vec3 eyePosition;

uniform vec4 ambientMaterial;
uniform vec4 diffuseMaterial;
uniform vec4 specularMaterial;
uniform vec4 emissiveMaterial;
uniform float shininess;

out vec4 fragColor;

void main() {
	vec4 ambience = vec4(ambientLight,1) * ambientMaterial;

	vec4 emissive = emissiveMaterial;

	vec3 lightDir = normalize(sunDirection - pos);
	float diff = max(dot(norm, lightDir), 0.0);

	vec4 diffuse = vec4(diff * sunLightColor, 1.0) * diffuseMaterial;

	vec3 eyeDir = normalize(eyePosition - pos);
	vec3 halfDir = normalize(lightDir + eyeDir);

	float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
	vec4 specular = vec4(spec * sunLightColor, 1) * specularMaterial;

	fragColor = emissive + ambience + diffuse + specular;
}
