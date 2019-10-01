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
        std::string id;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint VAO = 0, VBO = 0, EBO = 0;
        bool loaded = false;
        bool initialized = false;

    public:
        Model() {};
        Model(const std::string & file);
        void init();
        void render();
        void cleanUp();
        bool hasBeenLoaded() { return this->loaded; };
        bool hasBeenInitialized() { return this->initialized; };
};

class Entity {
    private:
        Model model;
        glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec2 rotation = glm::vec2(0.0f, 0.0f);

    public:
        Entity(const Model & model);
        glm::vec3 getPosition() { return this->position; };
        glm::vec2 getRotation()  { return this->rotation; };
        void setPosition(float x, float y, float z) {
            this->position.x = x;
            this->position.y = y;
            this->position.z = z;
        }
        void setRotationX(const int degrees) {
            this->rotation.x = glm::radians(static_cast< float >(degrees));
        }
        void setRotationY(const int degrees) {
            this->rotation.y = glm::radians(static_cast< float >(degrees));
        }
        glm::mat4 getTransformationMatrix();
        void setPosition();
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
