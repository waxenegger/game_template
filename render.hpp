#ifndef RENDER_HPP
#define RENDER_HPP

#include "includes.hpp"

class Vertex {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        Vertex(glm::vec3 position) {
            this->position = position;
        }
    };

class Model {
    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint VAO = 0, VBO = 0, EBO = 0;

        void init();
    public:
        Model() {};
        Model(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
        static Model * loadModelFromObjectFile(const std::string & file);
        void render();
        void cleanUp();
};

class Entity {
    private:
        Model model;

    public:
        Entity(const Model & model);
        void render();
        void cleanUp();
};

class Shader final {
    private:
        static const int NUM_SHADERS = 2;
        std::string m_file_name;
        GLuint m_program;
        GLuint m_shaders[NUM_SHADERS];

        std::string read(const int type) const;
        void checkForError(GLuint shader, GLuint flag, bool isProgram,
            const std::string & errorMessage) const;
        GLuint create(unsigned int type) const;

    public:
        Shader(const std::string & file_name);
        virtual ~Shader();
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, const glm::vec2 &value) const;
        void setVec2(const std::string &name, float x, float y) const;
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        void setVec4(const std::string &name, const glm::vec4 &value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w);
        void setMat2(const std::string &name, const glm::mat2 &mat) const;
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
        GLuint getId() const;
        void use() const;
};

#endif
