#include "render.hpp"

class Model;

Entity::Entity(std::shared_ptr<Model> model) : Entity() {
	this->model = model;
}

Entity::Entity(std::shared_ptr<Model> model, Shader * shader) : Entity(model) {
    this->useShader(shader);
}

void Entity::render() {
	if (this->model == nullptr || this->shader == nullptr) return;

	this->shader->use();
	if (this->shader->isBeingUsed()) {
	    this->shader->setMat4("model", this->calculateTransformationMatrix());

	    this->model->render(this->shader);

	    this->shader->stopUse();
	}
}

void Entity::cleanUp() { if (this->model != nullptr) this->model->cleanUp(); }
