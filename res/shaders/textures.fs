#version 330 core

in vec3 norm;
in vec3 pos;
in vec2 uvCoords;

uniform vec3 ambientLight;
uniform vec4 objectColor;
uniform vec3 sunDirection;
uniform vec3 sunLightColor;

uniform sampler2D texture_ambient;
uniform bool has_texture_ambient;
uniform sampler2D texture_diffuse;
uniform bool has_texture_diffuse;
uniform sampler2D texture_specular;
uniform bool has_texture_specular;
uniform sampler2D texture_normals;
uniform bool has_texture_normals;

out vec4 fragColor;

void main() {
	vec3 lightDir = normalize(sunDirection - pos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseLight = diff * sunLightColor;

	if (!has_texture_ambient && !has_texture_diffuse && !has_texture_specular) {
		fragColor = normalize(vec4(ambientLight + diffuseLight, 1.0) * objectColor);
	} else {
		vec4 ambientTexture = vec4(0.0);
		if (has_texture_ambient) {
			ambientTexture = texture(texture_ambient, uvCoords);
		}

		vec4 diffuseTexture = vec4(0.0);
		if (has_texture_diffuse) {
			diffuseTexture = texture(texture_diffuse, uvCoords);
		}

		vec4 specularTexture = vec4(0.0);
		if (has_texture_specular) {
			specularTexture = texture(texture_specular, uvCoords);
		}

		vec4 normalsTexture = vec4(0.0);
		if (has_texture_normals) {
			normalsTexture = texture(texture_normals, uvCoords);
		}

		vec4 lightContribution = normalize(vec4(ambientLight + diffuseLight, 1.0));
		vec4 textures = normalize(ambientTexture + diffuseTexture + specularTexture + normalsTexture * 0.6);

		fragColor = textures * lightContribution;
	}
}
