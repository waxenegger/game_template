#include "state.hpp"

RenderableGroup::RenderableGroup(std::string id) {
    this->id = id;
}

RenderableGroup::~RenderableGroup() {
    for (auto * renderable : this->content) delete renderable;
}

void RenderableGroup::addRenderable(Renderable * renderable) {
    if (renderable != nullptr) this->content.push_back(renderable);
}

void RenderableGroup::render() {
    if (this->content.size() == 0) return;

    std::vector<glm::mat4> modelMatrices;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    Shader * shader = nullptr;

    for (auto & renderable : this->content) {
        modelMatrices.push_back(renderable->calculateTransformationMatrix());
        materials.push_back(renderable->getMaterial());
        meshes = renderable->getMeshes();
        shader = renderable->getShader();
    }

    GLuint MODEL_MATRIX = 0, MATERIALS = 0;

    glGenBuffers(1, &MODEL_MATRIX);
    glBindBuffer(GL_ARRAY_BUFFER, MODEL_MATRIX);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &MATERIALS);
    glBindBuffer(GL_ARRAY_BUFFER, MATERIALS);
    glBufferData(GL_ARRAY_BUFFER, materials.size() * sizeof(Material), &materials[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, emissiveColor));

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, ambientColor));

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, diffuseColor));

    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, specularColor));

    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Material, shininess));

    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);

    for (auto & m : meshes) {
        m.render(shader);
        glDrawElementsInstanced(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0, modelMatrices.size());
        glBindVertexArray(0);
    }

    glDeleteBuffers(1, &MODEL_MATRIX);
    glDeleteBuffers(1, &MATERIALS);
}
