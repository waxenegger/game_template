#include "render.hpp"

class Model;

Entity::Entity(Model * model) : Entity() {
	if (model != nullptr) {
		if (this->model != nullptr) delete this->model;
		this->model = model;
	}
}

Entity::Entity(Model * model, Shader * shader) : Entity(model) {
    this->setShader(shader);
}

void Entity::setShader(Shader * shader) {
    if (this->model != nullptr) this->model->useShader(shader);
}

void Entity::render() {
	if (this->model == nullptr || this->model->getShader() == nullptr) return;

	this->model->getShader()->use();
	if (this->model->getShader()->isBeingUsed()) {
	    this->model->getShader()->setMat4("model", this->calculateTransformationMatrix());

	    this->model->render();

	    this->model->getShader()->stopUse();
	}
}

void Entity::cleanUp() { if (this->model != nullptr) this->model->cleanUp(); }

Entity::~Entity() {
	if (this->model != nullptr) delete this->model;
};
