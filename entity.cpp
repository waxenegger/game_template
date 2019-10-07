#include "game.hpp"
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
	if (shader != nullptr) {
		if (this->shader != nullptr) delete this->shader;
		this->shader = shader;
	}
}

void Entity::render() {
	if (this->shader != nullptr) {
		this->shader->use();
		if (this->shader->isBeingUsed()) {
			this->shader->setMat4("model", this->calculateTransformationMatrix());
			this->shader->setMat4("view", Camera::instance()->getViewMatrix());
			this->shader->setMat4("projection", Camera::instance()->getPerspective());
			this->shader->setVec4("lightColor",  1.0f, 1.0f, 1.0f, 1.0f);
			this->shader->setVec4("objectColor", this->color);
			//this->dumpActiveShaderAttributes();
		}
	}

    if (this->model != nullptr) this->model->render(this->shader);

    if (this->shader != nullptr) this->shader->stopUse();
}

void Entity::cleanUp() { if (this->model != nullptr) this->model->cleanUp(); }

glm::mat4 Entity::calculateTransformationMatrix() {
    glm::mat4 transformation = glm::mat4(1.0f);
    if (this->rotation.x != 0.0f) transformation = glm::rotate(transformation, this->rotation.x, glm::vec3(1, 0, 0));
    if (this->rotation.y != 0.0f) transformation = glm::rotate(transformation, this->rotation.y, glm::vec3(0, 1, 0));
    if (this->rotation.z != 0.0f) transformation = glm::rotate(transformation, this->rotation.z, glm::vec3(0, 0, 1));
    transformation = glm::translate(transformation, this->position);
    transformation = glm::scale(transformation, glm::vec3(this->scaleFactor));
    return transformation;
}

void Entity::dumpActiveShaderAttributes() {
    GLint i;
    GLint count;

    GLint size;
    GLenum type;

    const GLsizei bufSize = 16;
    GLchar name[bufSize];
    GLsizei length;

    std::cout << "Shader: " + std::to_string(this->shader->getId()) << std::endl;

    glGetProgramiv(this->shader->getId(), GL_ACTIVE_ATTRIBUTES, &count);
    std::cout << "Active Attributes: " + std::to_string(count) << std::endl;
    for (i = 0; i < count; i++) {
        glGetActiveAttrib(this->shader->getId(), (GLuint)i, bufSize, &length, &size, &type, name);
        std::cout << "Attribute " + std::to_string(i) + " Type: " + std::to_string(type) + " Name: " + name << std::endl;
    }

    glGetProgramiv(this->shader->getId(), GL_ACTIVE_UNIFORMS, &count);
    std::cout << "Active Uniforms: " + std::to_string(count) << std::endl;
    for (i = 0; i < count; i++) {
        glGetActiveUniform(this->shader->getId(), (GLuint)i, bufSize, &length, &size, &type, name);
        std::cout << "Uniform " + std::to_string(i) + " Type: " + std::to_string(type) + " Name: " + name << std::endl;
    }

}

Entity::~Entity() {
	if (this->model != nullptr) delete this->model;
	if (this->shader != nullptr) delete this->shader;
};
