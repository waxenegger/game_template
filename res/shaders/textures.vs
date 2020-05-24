#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 model;
layout (location = 6) in vec2 uvs;
layout (location = 7) in vec3 tangent;
layout (location = 8) in vec3 bitangent;

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

void main() {
	pos = vec3(view * model * vec4(position, 1.0));

    gl_Position = projection * vec4(pos, 1.0);

 	norm = normalize(mat3(transpose(inverse(model))) * normal);
    uvCoords = uvs;	

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
