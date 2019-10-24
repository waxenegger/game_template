#version 300 es
in vec3 position;
//in vec3 normal;
//in vec2 uv;\n
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
   	gl_Position = projection * view * model * vec4(position, 1.0);
    //text = uv;
    //norm = normalize(vec3(transpose(inverse(model)) * vec4(normal, 1.0f)));
}