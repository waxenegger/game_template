#include "render.hpp"

class Model;

Entity::Entity(std::shared_ptr<Model> model) : Entity() {
    this->id = model->getPath();
	this->model = model;
}

Entity::Entity(std::shared_ptr<Model> model, Shader * shader) : Entity(model) {
    this->useShader(shader);
}

void Entity::setMaterials(std::vector<Material> & materials) {
    if (this->model != nullptr) this->model->setMaterials(materials);
}

void Entity::setModelMatrices(std::vector<glm::mat4> & modelMatrices) {
    if (this->model != nullptr) this->model->setModelMatrices(modelMatrices);
}

void Entity::render() {
    if (this->model == nullptr || this->shader == nullptr) return;

    this->shader->use();
	if (this->shader->isBeingUsed()) {
	    this->model->render(this->shader);
	    this->shader->stopUse();
	}

}

void Entity::cleanUp() { if (this->model != nullptr) this->model->cleanUp(); }
