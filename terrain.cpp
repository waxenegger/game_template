#include "render.hpp"

Terrain::Terrain(const std::string & dir) {
    this->dir = dir;
    int start = -100, end = 100;
    int numberOfVertices = end - start;
    for (int row=start;row<end;row++) {
        for (int col=start;col<end;col++) {
            this->mesh.vertices.push_back(Vertex(glm::vec3(row, 0.0f, col)));
        }
    }
    unsigned int i = 0;
    for (int row=start;row<end-1;row++) {
        unsigned int j = 0;
        for (int col=start;col<end-1;col++) {
            this->mesh.indices.push_back(i * numberOfVertices + j);
            this->mesh.indices.push_back(i * numberOfVertices + j + 1);
            this->mesh.indices.push_back((i+1) * numberOfVertices + j);
            this->mesh.indices.push_back(i * numberOfVertices + j + 1);
            this->mesh.indices.push_back((i+1) * numberOfVertices + j);
            this->mesh.indices.push_back((i+1) * numberOfVertices + j + 1);
            j++;
        }
        i++;
    }

    this->initialized = true;
}

void Terrain::init() {
    this->mesh.init();

    this->terrainShader = new Shader(std::string(this->dir + "/res/terrain"));

    this->initialized = true;
}

void Terrain::render() {
    if (!this->initialized || this->terrainShader == nullptr) return;

    this->render(this->terrainShader);
}

void Terrain::render(Shader * shader) {
    if (!this->initialized) return;

    if (shader == nullptr) this->render();
    else {
        shader->use();
        if (shader->isBeingUsed()) {
            shader->setMat4("model", glm::mat4(1.0f));
            shader->setMat4("view", Camera::instance()->getViewMatrix());
            shader->setMat4("projection", Camera::instance()->getPerspective());
            shader->setVec4("ambientLight",  World::instance()->getAmbientLight());
            shader->setVec4("objectColor", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
            //shader->dumpActiveShaderAttributes();
        }
    }

    this->mesh.render(shader);

    if (shader != nullptr) shader->stopUse();
}

void Terrain::cleanUp() {
    if (!this->initialized) return;

    this->mesh.cleanUp();

    this->initialized = false;
}

Terrain::~Terrain() {
    if (this->terrainShader != nullptr) delete this->terrainShader;
}
