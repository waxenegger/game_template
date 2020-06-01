#include "render.hpp"

class Model;

Entity::Entity(std::shared_ptr<Model> model) : Entity() {
    this->id = model->getPath();
	this->model = model;
}

Entity::Entity(std::shared_ptr<Model> model, Shader * shader) : Entity(model) {
    this->useShader(shader);
}

void Entity::render() {
    if (this->model == nullptr || this->shader == nullptr) return;

	if (this->shader->isBeingUsed()) this->model->render(this->shader);
}

void Entity::cleanUp() { if (this->model != nullptr) this->model->cleanUp(); }
