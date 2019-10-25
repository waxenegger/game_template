#ifndef WORLD_HPP
#define WORLD_HPP

    #include "includes.hpp"

    class World final {
    private:
        static World * singleton;
        const float DEFAULT_AMBIENT_LIGHT_FACTOR = 0.5f;

        glm::vec3 ambientLight = glm::vec3(1.0f);
        glm::vec3 sunDirection = glm::vec3(0.001f, 0.0f, 0.001f);

        World();
    public:
        void setAmbientLightFactor(const float ambientLightFactor);
        glm::vec4 getAmbientLight();
        glm::vec3 getSunDirection();
        static World * instance();
};

#endif
