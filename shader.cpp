#include "render.hpp"

Shader::Shader() {
	this->init("");
    this->use();
}

void Shader::init(const std::string & file_name) {
    this->m_program = glCreateProgram();
    this->m_shaders[0] = this->create(GL_VERTEX_SHADER,
    		file_name.empty() ? DEFAULT_VERTEX_SHADER : this->read(GL_VERTEX_SHADER));
    this->m_shaders[1] = this->create(GL_FRAGMENT_SHADER,
    		file_name.empty() ? DEFAULT_FRAGMENT_SHADER : this->read(GL_FRAGMENT_SHADER));

    for (unsigned int i = 0; i < NUM_SHADERS; i++)
    glAttachShader(this->m_program, this->m_shaders[i]);

    glBindAttribLocation(m_program, 0, "position");

    glLinkProgram(m_program);
    this->checkForError(this->m_program, GL_LINK_STATUS, true,
            "Error linking shader program");

    glValidateProgram(m_program);
    this->checkForError(this->m_program, GL_LINK_STATUS, true,
            "Invalid shader program");
}

Shader::Shader(const std::string & file_name) {
    this->m_file_name = file_name;
    this->init(this->m_file_name);
}

std::string Shader::read(const int type) const {
    std::ifstream file;
    std::string prefixed_file_name = this->m_file_name;
    if (type == GL_FRAGMENT_SHADER)
        prefixed_file_name += ".fs";
    else
        prefixed_file_name += ".vs";
    file.open(prefixed_file_name.c_str());

    std::string output;
    std::string line;
    if (file.is_open()) {
        while (file.good()) {
            getline(file, line);
            output.append(line + "\n");
        }
        //cout << output << endl;
    } else
        std::cerr << "Unable to read shader: " << this->m_file_name
                << std::endl;

    return output;
}

void Shader::checkForError(GLuint shader, GLuint flag, bool isProgram,
        const std::string & errorMessage) {
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if (isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if (success == GL_FALSE) {
        if (isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
        this->loaded = false;
    }
}

GLuint Shader::create(const unsigned int type, const std::string text) {
    GLuint shader = glCreateShader(type);

    if (shader == 0)
        std::cerr << "Error compiling shader type " << type << std::endl;

    const GLchar* p[1];
    p[0] = text.c_str();
    GLint lengths[1];
    lengths[0] = text.length();

    glShaderSource(shader, 1, p, lengths);
    glCompileShader(shader);

    this->checkForError(shader, GL_COMPILE_STATUS, false,
            "Error compiling shader!");

    return shader;
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(this->m_program, name.c_str()),
            (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(this->m_program, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(this->m_program, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(this->m_program, name.c_str()), 1,
            &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(this->m_program, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(this->m_program, name.c_str()), 1,
            &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(this->m_program, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(this->m_program, name.c_str()), 1,
            &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(this->m_program, name.c_str()), x, y, z,
            w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(this->m_program, name.c_str()), 1,
            GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(this->m_program, name.c_str()), 1,
            GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(this->m_program, name.c_str()), 1,
            GL_FALSE, &mat[0][0]);
}

GLuint Shader::getId() const {
    return this->m_program;
}

void Shader::use() {
	if (this->loaded) {
		glUseProgram(this->m_program);
		this->used = true;
	}
}

void Shader::stopUse() {
	if (this->loaded && this->used) {
		glUseProgram(0);
		this->used = false;
	}
}

Shader:: ~Shader() {
    for (unsigned int i = 0; i < NUM_SHADERS; i++) {
        glDetachShader(this->m_program, this->m_shaders[i]);
        glDeleteShader(this->m_shaders[i]);
    }

    glDeleteProgram(this->m_program);
}
