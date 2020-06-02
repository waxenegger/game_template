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

    Renderable * firstRenderable = this->content[0];

    std::vector<Material> materials;
    std::vector<glm::mat4> modelMatrices;

    for (auto & renderable : this->content) {
        modelMatrices.push_back(renderable->calculateTransformationMatrix());
        materials.push_back(renderable->getMaterial());
    }

    firstRenderable->setMaterials(materials);
    firstRenderable->setModelMatrices(modelMatrices);

    firstRenderable->render();
}
