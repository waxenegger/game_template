#version 300 es

in vec3 position;
in vec3 normal;
//in vec2 uv;\n

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 norm;
out vec3 pos;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0);
    pos = normalize(vec3(model * vec4(position, 1.0f)));
    norm = normalize(vec3(transpose(inverse(model)) * vec4(normal, 1.0f)));
    //norm = normal;
    //text = uv;
}