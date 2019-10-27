#include "world.hpp"

World::World() {
    this->ambientLight *= DEFAULT_AMBIENT_LIGHT_FACTOR;
    this->sunLightColor *= DEFAULT_SUNLIGHT_STRENGTH;
};

void World::setAmbientLightFactor(const float ambientLightFactor) {
    if (ambientLightFactor > 0.0f && ambientLightFactor <= 1.0f)
        this->ambientLight = glm::vec3(1.0f) * ambientLightFactor;
};

void World::setSunLightStrength(const float sunLightStrength) {
    if (sunLightStrength > 0.0f && sunLightStrength <= 1.0f)
        this->sunLightColor = glm::vec3(1.0f) * sunLightStrength;
};

glm::vec4 World::getSunLightColor() {
    return glm::vec4(this->sunLightColor, 1.0f);
};

glm::vec4 World::getAmbientLight() {
    return glm::vec4(this->ambientLight, 1.0f);
};

glm::vec3 World::getSunDirection() {
    return this->sunDirection;
};

World * World::instance() {
    if (World::singleton == nullptr) World::singleton = new World();
    return World::singleton;
}

World * World::singleton = nullptr;
