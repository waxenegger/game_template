#version 330 core

in vec3 position;
in vec3 normal;
in vec2 uvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 norm;
out vec3 pos;
out vec2 uvCoords;

void main() {
	pos = vec3(view * model * vec4(position, 1.0));
    gl_Position = projection * vec4(pos, 1.0);
    norm = normalize(mat3(transpose(inverse(model))) * normal);
    uvCoords = uvs;
}
