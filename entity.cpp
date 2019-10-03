#include "game.hpp"
#include "render.hpp"

class Model;

Entity::Entity(const Model & model) { this->model = model; }

Entity::Entity(const Model & model, const Shader & shader) : Entity(model) {
	this->setShader(shader);
}

void Entity::render(Camera & camera) {
	this->shader.use();

	if (this->shader.isBeingUsed()) {
		this->shader.setMat4("model", this->calculateTransformationMatrix());
		this->shader.setVec3("objectColor", 1.0f, 1.0f, 0.0f);
		camera.setShaderUniforms(this->shader);
	}

	this->model.render();

	this->shader.stopUse();
}

void Entity::cleanUp() { this->model.cleanUp(); }

glm::mat4 Entity::calculateTransformationMatrix() {
	glm::mat4 transformation = glm::mat4(1.0f);
	if (this->rotation.x != 0.0f) transformation = glm::rotate(transformation, this->rotation.x, glm::vec3(1, 0, 0));
	if (this->rotation.x != 0.0f) transformation = glm::rotate(transformation, this->rotation.y, glm::vec3(0, 1, 0));
	if (this->rotation.z != 0.0f) transformation = glm::rotate(transformation, this->rotation.z, glm::vec3(0, 0, 1));
	transformation = glm::translate(transformation, this->position);
	transformation = glm::scale(transformation, glm::vec3(this->scaleFactor));
    return transformation;
}
