#ifndef WORLD_HPP
#define WORLD_HPP

    #include "includes.hpp"

    class World final {
    private:
        static World * singleton;
        const float DEFAULT_AMBIENT_LIGHT_FACTOR = 0.05f;
        const float DEFAULT_SUNLIGHT_STRENGTH = 0.75f;
        const glm::vec3 DEFAULT_SUNLIGHT_COLOR = glm::vec3(1.0f);
        const glm::vec3 DEFAULT_AMBIENT_LIGHT_COLOR = glm::vec3(1.0f);

        bool gravity = true;

        glm::vec3 ambientLight = DEFAULT_AMBIENT_LIGHT_COLOR;
        glm::vec3 sunDirection = glm::vec3(20.0f, 100.0f, 15.0f);
        glm::vec3 sunLightColor = DEFAULT_SUNLIGHT_COLOR;

        World();
    public:
        void toggleGravity();
        bool hasGravity();
        void setAmbientLightFactor(const float ambientLightFactor);
        void setSunLightStrength(const float sunLightStrength);
        glm::vec3 getAmbientLight();
        glm::vec3 getSunDirection();
        glm::vec3 getSunLightColor();
        static World * instance();
};

#endif
