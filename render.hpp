#ifndef RENDER_HPP
#define RENDER_HPP

#include "includes.hpp"
#include "world.hpp"

static const int DEFAULT_WIDTH = 640;
static const int DEFAULT_HEIGHT = 480;

class Texture {
    private:
        unsigned int id = 0;
        std::string type;
        std::string path;
    public:
        unsigned int getId() {
            return  this->id;
        }
        std::string getType() {
            return this->type;
        }
        std::string getPath() {
            return this->path;
        }
        void setId(const unsigned int & id) {
            this->id = id;
        }
        void setType(const std::string & type) {
            this->type = type;
        }
        void setPath(const std::string & path) {
            this->path = path;
        }
        void cleanUp() {
            glDeleteTextures(1, &this->id);
        }
};

static std::map<std::string, Texture> TEXTURES;

class Material {
    public:
        glm::vec4 ambientColor = glm::vec4(0.2f,0.2f,0.2f,1.0f);
        glm::vec4 emissiveColor = glm::vec4(0.1f, 0.1f, 0.1f,1.0f);
        glm::vec4 diffuseColor = glm::vec4(0.9f,0.9f,0.9f,1.0f);
        glm::vec4 specularColor = glm::vec4(0.1f,0.1f,0.1f,1.0f);
        float shininess = 1.0f;
        Material() {};
};

static const int NUM_SHADERS = 2;
static const std::string DEFAULT_VERTEX_SHADER =
        "#version 330 core\n"
        "in vec3 position;\n"
        "in vec3 normal;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "out vec3 norm;\n"
        "out vec3 pos;\n"
        "void main() {\n"
        "    pos = vec3(view * model * vec4(position, 1.0));\n"
        "    gl_Position = projection * vec4(pos, 1.0);\n"
        "    norm = normalize(mat3(transpose(inverse(model))) * normal);\n"
        "}";
static const std::string DEFAULT_FRAGMENT_SHADER =
        "#version 330 core\n"
        "in vec3 norm;\n"
        "in vec3 pos;\n"
        "uniform vec4 ambientMaterial;\n"
        "uniform vec4 diffuseMaterial;\n"
        "uniform vec4 specularMaterial;\n"
        "uniform vec4 emissiveMaterial;\n"
        "uniform float shininess;\n"
        "uniform vec3 ambientLight;\n"
        "uniform vec3 sunDirection;\n"
        "uniform vec3 sunLightColor;\n"
        "uniform vec3 eyePosition;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "    vec4 ambience = vec4(ambientLight,1) * ambientMaterial;\n"
        "    vec4 emissive = emissiveMaterial;\n"
        "    vec3 lightDir = normalize(sunDirection - pos);\n"
        "    float diff = max(dot(norm, lightDir), 0.0);\n"
        "    vec4 diffuse = vec4(diff * sunLightColor, 1.0) * diffuseMaterial;\n"
        "    vec3 eyeDir = normalize(eyePosition - pos);\n"
        "    vec3 halfDir = normalize(lightDir + eyeDir);\n"
        "    float spec = pow(max(dot(norm, halfDir), 0.0), shininess);\n"
        "    vec4 specular = vec4(spec * sunLightColor, 1) * specularMaterial;\n"
        "    fragColor = emissive + ambience + diffuse + specular;\n"
        "}";

class Shader final {
    private:
        std::string m_file_name;

        GLuint m_program = 0;
        GLuint m_shaders[NUM_SHADERS];

        bool loaded = true;
        bool used = false;

        std::string read(const int type) const;
        void checkForError(GLuint shader, GLuint flag, bool isProgram,
                const std::string & errorMessage);
        GLuint create(const unsigned int type, const std::string text);
        void init(const std::string & file_name);

    public:
        Shader();
        Shader(const std::string & file_name);
        virtual ~Shader();
        bool hasBeenLoaded() {
            return this->loaded;
        };
        bool isBeingUsed() {
            return this->used;
        };
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setIntVec(const std::string &name, std::vector<GLint> value) const;
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
        void stopUse();
        void dumpActiveShaderAttributes();
};

class Vertex {
public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    glm::vec3 tangent;
    glm::vec3 bitTangent;

    glm::vec3 colorAmbient;
    glm::vec3 colorDiffuse;
    glm::vec3 colorSpecular;

    Vertex(glm::vec3 position) {
        this->position = position;
    }
};

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        Material material;
        GLuint VAO = 0, VBO = 0, EBO = 0;

        Mesh() {

        };

        Mesh(std::vector<Vertex> & vertices, std::vector<unsigned int> & indices, std::vector<Texture> & textures) {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;
        }
        void init();
        void render(Shader * shader);
        void cleanUp();
};

class Terrain {
    private:
        std::string dir;
        Mesh mesh;
        Shader * terrainShader = nullptr;
        bool initialized = false;
    public:
        Terrain(const std::string & dir);
        void init();
        void render();
        void render(Shader * shader);
        bool hasBeenInitialized() {
            return this->initialized;
        };

        void cleanUp();
        ~Terrain();
};

class Model {
    private:
        std::string file;
        std::string dir;
        std::vector<Mesh> meshes;
        bool loaded = false;
        bool initialized = false;

        void processNode(const aiNode * node, const aiScene *scene);
        Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
        void addTextures(const aiMaterial * mat, const aiTextureType type, const std::string name, std::vector<Texture> & textures);
        void correctTexturePath(char * path);
    public:
        static const std::string AMBIENT_TEXTURE;
        static const std::string DIFFUSE_TEXTURE;
        static const std::string SPECULAR_TEXTURE;
        static const std::string TEXTURE_NORMALS;
        ~Model() { this->cleanUp();}
        Model() {};
        Model(const std::string & dir, const std::string & file);
        void init();
        void render(Shader * shader);
        void cleanUp();
        bool hasBeenLoaded() {
            return this->loaded;
        };
        bool hasBeenInitialized() {
            return this->initialized;
        };
        void useMaterial(const Material & material);
};

class Camera final {
    private:
		static Camera * singleton;
        const float SENSITIVITY_DIRECTION_CHANGE = 0.0005f;
        const float SENSITIVITY_POSITION_CHANGE = 0.05f;

        float pitch = glm::asin(1);
        float yaw = glm::atan(0.0f, 0.0f);
        float fovy = 45.0f;

        glm::vec3 position = glm::vec3(1.0f);
        glm::vec3 direction = glm::vec3(0.001f, 0.0f, 0.001f);
        glm::mat4 perspective = glm::perspective(glm::radians(fovy),
                static_cast<float>(DEFAULT_WIDTH)
                / static_cast<float>(DEFAULT_HEIGHT), 0.1f, 10000.0f);
        const glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        Camera() {};
        Camera(const float x, const float y, const float z);
    public:
        void setPosition(const float x, const float y, const float z);
        glm::vec3 getPosition();
        void setDirection(const float x, const float y, const float z);
        glm::vec3 getDirection();
        void setPerspective(const glm::mat4 perspective);
        glm::mat4 getPerspective();
        void updateDirection(const float deltaX, const float deltaY,
                const float frameDuration);
        void updateLocation(const char direction, const float frameDuration);
        glm::mat4 getViewMatrix();
        float getFieldOfViewY();
        void setFieldOfViewY(const float fovy);
        static Camera * instance() {
        	if (Camera::singleton == nullptr) Camera::singleton = new Camera();
        	return Camera::singleton;
        }
        static Camera * instance(const float x, const float y, const float z) {
        	if (Camera::singleton == nullptr) Camera::singleton = new Camera(x,y,z);
        	return Camera::singleton;
        }
};

class Entity {
    private:
        Model * model = nullptr;
        Shader * shader = nullptr;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        float scaleFactor = 1.0f;

    public:
        ~Entity();
        Entity() { this->shader = new Shader();};
        Entity(Model * model);
        Entity(Model * model, Shader * shader);
        void setShader(Shader * shader);
        float getScaleFactor() {
            return this->scaleFactor;
        }
        void setScaleFactor(const float scaleFactor) {
            this->scaleFactor = scaleFactor;
        };
        glm::vec3 getPosition() {
            return this->position;
        };
        glm::vec3 getRotation() {
            return this->rotation;
        };
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
        void setColor(const float red, const float green, const float blue, const float alpha) {
            if (this->model == nullptr) return;

            Material newColorMaterial;
            newColorMaterial.diffuseColor = glm::vec4(red, green, blue, alpha);
            this->model->useMaterial(newColorMaterial);
        }
};

#endif
