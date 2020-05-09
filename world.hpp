#ifndef WORLD_HPP
#define WORLD_HPP

    #include "includes.hpp"

    class World final {
    private:
        static World * singleton;
        const float DEFAULT_AMBIENT_LIGHT_FACTOR = 0.15f;
        const float DEFAULT_SUNLIGHT_STRENGTH = 0.75f;

        glm::vec3 ambientLight = glm::vec3(1.0f);
        glm::vec3 sunDirection = glm::vec3(-25.0f, 100.0f, -25.0f);
        glm::vec3 sunLightColor = glm::vec3(1.0f);

        World();
    public:
        void setAmbientLightFactor(const float ambientLightFactor);
        void setSunLightStrength(const float sunLightStrength);
        glm::vec3 getAmbientLight();
        glm::vec3 getSunDirection();
        glm::vec3 getSunLightColor();
        static World * instance();
};

#endif
