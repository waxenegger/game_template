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

class Mesh {
    public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		GLuint VAO = 0, VBO = 0, EBO = 0;

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
        void init();
        void render();
        void cleanUp();
    };

class Model {
    private:
        std::string id;
        std::vector<Mesh> meshes;
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

class Shader final {
    static const int NUM_SHADERS = 2;

	private:
        std::string m_file_name;

        GLuint m_program = 0;
        GLuint m_shaders[NUM_SHADERS];

        bool loaded = true;
        bool used = false;

        std::string read(const int type) const;
        void checkForError(GLuint shader, GLuint flag, bool isProgram,
            const std::string & errorMessage);
        GLuint create(unsigned int type);

    public:
        Shader() { this->loaded = false; };
        Shader(const std::string & file_name);
        virtual ~Shader();
        bool hasBeenLoaded() { return this->loaded; };
        bool isBeingUsed() { return this->used; };
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
        void use();
};

class Entity {
    private:
        Model model;
        Shader shader;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        float scaleFactor = 1.0f;

    public:
        Entity() = delete;
        Entity(const Model & model);
        Entity(const Model & model, const Shader & shader);
        void setShader(const Shader & shader)  { this->shader = shader; };
        float getScaleFactor() { return this->scaleFactor; };
        void setScaleFactor(const float scaleFactor)  { this->scaleFactor = scaleFactor; };
        glm::vec3 getPosition() { return this->position; };
        glm::vec3 getRotation()  { return this->rotation; };
        void setPosition(const float x, const float y, const float z) {
            this->position.x = x;
            this->position.y = y;
            this->position.z = z;
        }
        void setRotation(const int x = 0, const int y = 0, const int z = 0) {
            this->rotation.x = glm::radians(static_cast<float>(x));
            this->rotation.y = glm::radians(static_cast<float>(y));
            this->rotation.z = glm::radians(static_cast<float>(z));
        }
        glm::mat4 calculateTransformationMatrix();
        void render();
        void cleanUp();
};

#endif
