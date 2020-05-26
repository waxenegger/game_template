#version 330 core

in vec3 pos;
in vec3 norm;

in vec2 uvCoords;

in vec4 emissiveColor;
in vec4 ambientColor;
in vec4 diffuseColor;
in vec4 specularColor;
in float shininess;

in vec3 sunPos;
in vec3 eyePos;

uniform vec3 ambientLight;
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
	vec3 normals = norm;
	if (has_texture_normals) {
		normals = texture(texture_normals, uvCoords).rgb;
		normals = normalize(normals * 2.0 - 1.0);
	}

	vec4 ambience = vec4(ambientLight,1) * ambientColor;
	if (has_texture_ambient) {
		ambience *= texture(texture_ambient, uvCoords);
	}

	vec3 lightDir = normalize(sunPos - pos);
	float diff = max(dot(normals, lightDir), 0.0);

	vec4 diffuse = vec4(diff * sunLightColor, 1.0) * diffuseColor;
	if (has_texture_diffuse) {
		diffuse *= texture(texture_diffuse, uvCoords);
	}

	vec3 eyeDir = normalize(eyePos - pos);
	vec3 halfDir = normalize(lightDir + eyeDir);

	float spec = pow(max(dot(normals, halfDir), 0.0), shininess);
	vec4 specular = vec4(spec * sunLightColor, 1) * specularColor;
	if (has_texture_specular) {
		specular *= texture(texture_specular, uvCoords);
	}

	fragColor = emissiveColor + ambience + diffuse + specular;
}
