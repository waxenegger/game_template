#include "render.hpp"

Camera::Camera(const float x, const float y, const float z) {
    this->position = glm::vec3(x, y, z);
}
;
void Camera::setPosition(const float x, const float y, const float z) {
    this->position = glm::vec3(x, y, z);
}
;
glm::vec3 Camera::getPosition() {
    return this->position;
}
;
void Camera::setDirection(const float x, const float y, const float z) {
    this->direction = glm::normalize(glm::vec3(x, y, z));
}
;
glm::vec3 Camera::getDirection() {
    return this->direction;
}
;
void Camera::setPerspective(const glm::mat4 perspective) {
    this->perspective = perspective;
}
void Camera::updateDirection(const float deltaX, const float  deltaY, const float frameDuration) {

    this->pitch = glm::asin(this->getDirection().y) -
        deltaY * SENSITIVITY_DIRECTION_CHANGE * frameDuration;

    this->yaw = glm::atan(this->getDirection().z, this->getDirection().x) +
        deltaX * SENSITIVITY_DIRECTION_CHANGE * frameDuration;

    this->setDirection(
        glm::cos(this->pitch) * glm::cos(this->yaw), glm::sin(this->pitch), glm::cos(this->pitch) * glm::sin(this->yaw));

}

void Camera::updateLocation(const char direction, const float frameDuration) {
    if (direction != 'A' && direction != 'W' &&
            direction != 'D' && direction != 'S') return;

    const float speedFactor = SENSITIVITY_POSITION_CHANGE * frameDuration;

    this->pitch = glm::asin(this->getDirection().y);
    this->yaw = glm::atan(this->getDirection().z, this->getDirection().x);

    float deltaXRot = glm::cos(this->pitch) * glm::cos(this->yaw);
    float deltaYRot = glm::cos(this->pitch) * glm::sin(this->yaw);

    float deltaXMove = glm::cos(this->yaw - glm::pi<float>() / 2);
    float deltaYMove = glm::sin(this->yaw - glm::pi<float>() / 2);

    switch(direction) {
        case 'W':
            this->position.x += deltaXRot * speedFactor;
            this->position.z += deltaYRot * speedFactor;
            break;
        case 'S':
            this->position.x -= deltaXRot * speedFactor;
            this->position.z -= deltaYRot * speedFactor;
            break;
        case 'A':
            this->position.x += deltaXMove * speedFactor;
            this->position.z += deltaYMove * speedFactor;
            break;
        case 'D':
            this->position.x -= deltaXMove * speedFactor;
            this->position.z -= deltaYMove * speedFactor;
            break;
    }
}

void Camera::setShaderUniforms(Shader & shader) {
    shader.setMat4("view",
            glm::lookAt(this->position, this->position + this->direction,
                    this->upVector));
    shader.setMat4("projection", this->perspective);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
}
