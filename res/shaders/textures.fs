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

	vec4 texNormals = vec4(norm,1);
	if (has_texture_normals) {
		texNormals = normalize(texture(texture_normals, uvCoords));
	}
	float diff = max(dot(texNormals.xyz, lightDir), 0.0);
	vec3 diffuseLight = diff * sunLightColor;

	if (!has_texture_ambient && !has_texture_diffuse && !has_texture_specular) {
		fragColor = normalize(vec4(ambientLight + diffuseLight, 1.0) * objectColor);
	} else {
		vec4 finalTexture = vec4(0.0);

		if (has_texture_ambient) {
			finalTexture = texture(texture_ambient, uvCoords);
		}

		if (has_texture_diffuse) {
			if (finalTexture == vec4(0.0)) {
				finalTexture = texture(texture_diffuse, uvCoords);
			} else {
				finalTexture = mix(finalTexture, texture(texture_diffuse, uvCoords), 0.8);
			}
		}

		if (has_texture_specular) {
			if (finalTexture == vec4(0.0)) {
				finalTexture = texture(texture_specular, uvCoords);
			} else {
				finalTexture = mix(finalTexture, texture(texture_specular, uvCoords), 0.8);
			}
		}

		vec4 lightContribution = normalize(vec4(ambientLight + diffuseLight, 1.0));
		fragColor = finalTexture * lightContribution;
	}
}
