#include "render.hpp"

Terrain::Terrain() {
    this->initialized = true;
    this->mesh.vertices.push_back(
            Vertex(glm::vec3(0.0f, 0.0f, 0.0f)));
    this->mesh.indices.push_back(0);
    this->mesh.vertices.push_back(
            Vertex(glm::vec3(5.0f, 0.0f, 5.0f)));
    this->mesh.indices.push_back(1);
    this->mesh.vertices.push_back(
            Vertex(glm::vec3(-5.0f, 0.0f, -5.0f)));
    this->mesh.indices.push_back(2);
}

void Terrain::init() {
    this->mesh.init();

    this->terrainShader = new Shader();

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
            shader->setVec4("lightColor",  1.0f, 1.0f, 1.0f, 1.0f);
            shader->setVec4("objectColor", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
            shader->dumpActiveShaderAttributes();
        }
    }

    this->mesh.render(shader);
}

void Terrain::cleanUp() {
    if (!this->initialized) return;

    this->mesh.cleanUp();

    this->initialized = false;
}

Terrain::~Terrain() {
    if (this->terrainShader != nullptr) delete this->terrainShader;
}
