#version 330 core

in vec3 position;
in vec3 normal;
in vec2 uvs;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 model;
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
	    T = normalize(T - dot(T, N) * N);    
	    mat3 TBN = transpose(mat3(T, cross(N, T), N));    
	    
	    pos = TBN * pos;
	    eyePos = TBN * eyePos;
	    sunPos = TBN * sunPos;	
	}
}
