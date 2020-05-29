#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 model;
layout (location = 6) in vec4 emissiveMaterial;
layout (location = 7) in vec4 ambientMaterial;
layout (location = 8) in vec4 diffuseMaterial;
layout (location = 9) in vec4 specularMaterial;
layout (location = 10) in float shininessMaterial;
layout (location = 11) in vec2 uvs;
layout (location = 12) in vec3 tangent;
layout (location = 13) in vec3 bitangent;

uniform mat4 view;
uniform mat4 projection;

uniform bool has_texture_normals;
uniform vec3 sunDirection;
uniform vec3 eyePosition;

out vec3 pos;
out vec3 norm;
out vec3 sunPos;
out vec3 eyePos;
out vec2 uvCoords;
out vec4 emissiveColor;
out vec4 ambientColor;
out vec4 diffuseColor;
out vec4 specularColor;
out float shininess;

void main() {
	pos = vec3(model * vec4(position, 1.0));

    gl_Position = projection * view * vec4(pos, 1.0);

 	norm = normalize(mat3(transpose(inverse(model))) * normal);

    uvCoords = uvs;	
	emissiveColor = emissiveMaterial;
	ambientColor = ambientMaterial;
	diffuseColor = diffuseMaterial;
	specularColor = specularMaterial;
	shininess = shininessMaterial;

    eyePos = eyePosition;
    sunPos = sunDirection;	

	if (has_texture_normals) {
	    mat3 normalMat = mat3(transpose(inverse(model)));

	    vec3 T = normalize(normalMat * tangent);
	    vec3 N = normalize(normalMat * normal);
	    vec3 B = normalize(normalMat * bitangent);

	    T = normalize(T - dot(T, N) * N);
	    if (dot(cross(N, T), B) < 0.0f) T *= -1.0f;
	    mat3 TBN = transpose(mat3(T, cross(N, T), N));
	    
	    pos = TBN * pos;
	    eyePos = TBN * eyePos;
	    sunPos = TBN * sunPos;	
	}
}
