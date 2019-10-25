#include "world.hpp"

World::World() {
    this->ambientLight * DEFAULT_AMBIENT_LIGHT_FACTOR;
};

void World::setAmbientLightFactor(const float ambientLightFactor) {
    if (ambientLightFactor > 0.0f && ambientLightFactor <= 1.0f)
        this->ambientLight = glm::vec3(1.0f) * ambientLightFactor;
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
