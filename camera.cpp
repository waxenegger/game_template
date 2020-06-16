#include "render.hpp"

Camera::Camera(const float x, const float y, const float z) {
    this->position = glm::vec3(x, y, z);
};

void Camera::setPosition(const float x, const float y, const float z) {
    this->position.x = x;
    this->position.y = y;
    this->position.z = z;
};

glm::vec3 Camera::getPosition() {
    return this->position;
};

void Camera::setDirection(const float x, const float y, const float z) {
    const glm::vec3 tmp = glm::normalize(glm::vec3(x, y, z));
    this->direction.x = tmp.x;
    this->direction.y = tmp.y;
    this->direction.z = tmp.z;
};

glm::vec3 Camera::getDirection() {
    return this->direction;
};

void Camera::setPerspective(const glm::mat4 perspective) {
    this->perspective = perspective;
}

glm::mat4 Camera::getPerspective() {
    return this->perspective;
}

void Camera::updateDirection(const float deltaX, const float  deltaY, const float frameDuration) {

    this->pitch = glm::asin(this->getDirection().y) -
        deltaY * SENSITIVITY_DIRECTION_CHANGE * frameDuration;

    this->yaw = glm::atan(this->getDirection().z, this->getDirection().x) +
        deltaX * SENSITIVITY_DIRECTION_CHANGE * frameDuration;

    const float ninetyDegrees = glm::pi<float>() / 2;
    if (glm::abs(this->pitch) == 0.0f) this->pitch = 0.000001f;
    else if (glm::abs(this->pitch) >= ninetyDegrees) this->pitch = ninetyDegrees * (this->pitch < 0 ? -0.999999f : 0.999999f);

    if (glm::abs(this->yaw) == 0.0f) this->pitch = 0.000001f;
    else if (glm::abs(this->yaw) == ninetyDegrees) this->yaw *= (this->yaw < 0 ? -0.999999f : 0.999999f);

    this->setDirection(
        glm::cos(this->pitch) * glm::cos(this->yaw), glm::sin(this->pitch), glm::cos(this->pitch) * glm::sin(this->yaw));

}

void Camera::updateLocation(const SDL_Scancode & direction, const float frameDuration) {
    if (direction != SDL_SCANCODE_A && direction != SDL_SCANCODE_W && direction != SDL_SCANCODE_D && direction != SDL_SCANCODE_S) return;

    const float speedFactor = SENSITIVITY_POSITION_CHANGE * frameDuration;

    this->pitch = glm::asin(this->getDirection().y);
    this->yaw = glm::atan(this->getDirection().z, this->getDirection().x);

    float deltaXRot = glm::cos(this->yaw);
    float deltaYRot = glm::sin(this->yaw);

    float deltaXMove = glm::cos(this->yaw - glm::pi<float>() / 2);
    float deltaYMove = glm::sin(this->yaw - glm::pi<float>() / 2);

    switch(direction) {
        case SDL_SCANCODE_W:
            this->position.x += deltaXRot * speedFactor;
            this->position.z += deltaYRot * speedFactor;
            break;
        case SDL_SCANCODE_S:
            this->position.x -= deltaXRot * speedFactor;
            this->position.z -= deltaYRot * speedFactor;
            break;
        case SDL_SCANCODE_A:
            this->position.x += deltaXMove * speedFactor;
            this->position.z += deltaYMove * speedFactor;
            break;
        case SDL_SCANCODE_D:
            this->position.x -= deltaXMove * speedFactor;
            this->position.z -= deltaYMove * speedFactor;
            break;
        default:
            break;
    }
}

void Camera::startJumpFrameCounter() {
    this->jumpFrameCounter = 1;
}

void Camera::setJumpFrameCounter(const int jumpFrameCounter) {
    this->jumpFrameCounter = jumpFrameCounter;
}

void Camera::updateYlocation(const float frameDuration) {
    if (this->jumpFrameCounter > 0) this->position.y += frameDuration  * JUMP_FACTOR * this->jumpFrameCounter;

    if (World::instance()->hasGravity())
        this->position.y -= frameDuration * 0.5 * GRAVITY * static_cast<float>(glm::pow(this->jumpFrameCounter, 2));
    else this->jumpFrameCounter = -1;

    if (this->position.y < 7.0f) {
        this->position.y = 7.0f;
        this->jumpFrameCounter = -1;
    } else if (World::instance()->hasGravity() && this->jumpFrameCounter > 0) this->jumpFrameCounter++;
}

bool Camera::isOffGround() {
    return this->jumpFrameCounter > 0;
}

glm::mat4 Camera::getViewMatrix() {
    return
            glm::lookAt(this->position, this->position + this->direction,
                    this->upVector);
}

float Camera::getFieldOfViewY() {
    return this->fovy;
}

void Camera::setFieldOfViewY(const float fovy) {
    this->fovy = fovy;
}


Camera * Camera::singleton = nullptr;
