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

glm::mat4 Entity::calculateTransformationMatrix() {
    glm::mat4 transformation = glm::mat4(1.0f);

    transformation = glm::translate(transformation, this->position);

    transformation = glm::scale(transformation, glm::vec3(this->scaleFactor));

    if (this->rotation.x != 0.0f) transformation = glm::rotate(transformation, this->rotation.x, glm::vec3(1, 0, 0));
    if (this->rotation.y != 0.0f) transformation = glm::rotate(transformation, this->rotation.y, glm::vec3(0, 1, 0));
    if (this->rotation.z != 0.0f) transformation = glm::rotate(transformation, this->rotation.z, glm::vec3(0, 0, 1));

    return transformation;
}

Entity::~Entity() {
	if (this->model != nullptr) delete this->model;
};
