#include "state.hpp"

RenderableGroup::RenderableGroup(std::string id) {
    this->id = id;
}

RenderableGroup::~RenderableGroup() {
    glDeleteBuffers(1, &this->MODEL_MATRIX);
    glDeleteBuffers(1, &this->MATERIALS);

    for (auto * renderable : this->content) delete renderable;
}

void RenderableGroup::addRenderable(Renderable * renderable) {
    if (renderable != nullptr) this->content.push_back(renderable);
}

void RenderableGroup::render() {
    if (this->content.size() == 0) return;

    Renderable * firstRenderable = this->content[0];
    std::vector<Mesh> meshes = firstRenderable->getMeshes();
    Shader * shader = firstRenderable->getShader();

    this->modelMatrices.clear();
    this->materials.clear();
    for (auto & renderable : this->content) {
        this->modelMatrices.push_back(renderable->calculateTransformationMatrix());
        this->materials.push_back(renderable->getMaterial());
    }

    glGenBuffers(1, &this->MODEL_MATRIX);
    glBindBuffer(GL_ARRAY_BUFFER, this->MODEL_MATRIX);
    glBufferData(GL_ARRAY_BUFFER, this->modelMatrices.size() * sizeof(glm::mat4), &this->modelMatrices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &this->MATERIALS);
    glBindBuffer(GL_ARRAY_BUFFER, this->MATERIALS);
    glBufferData(GL_ARRAY_BUFFER, this->materials.size() * sizeof(Material), &this->materials[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, emissiveColor));

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, ambientColor));

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, diffuseColor));

    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, specularColor));

    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, shininess));

    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);

    if (shader != nullptr) {
        shader->use();

        firstRenderable->render();
        for (auto & m : meshes) {
            m.render(shader);
            glDrawElementsInstanced(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0, this->modelMatrices.size());
            glBindVertexArray(0);
        }
        shader->stopUse();
    }
}
