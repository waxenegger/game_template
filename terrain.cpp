#include "render.hpp"

Terrain::Terrain(const std::string & dir) {
    #ifdef WIN32
        ::srand( GetTickCount() );
    #else
        srand (static_cast <unsigned> (time(0)));
    #endif

    this->dir = dir;
    int start = -100, end = 100, step = 2;
    int numberOfVertices = (end - start) / step;
    for (int row=start;row<end;row+=step) {
        for (int col=start;col<end;col+=step) {
            const float randHeight = static_cast<const float>((rand() % 4));
            this->mesh.vertices.push_back(Vertex(glm::vec3(row, randHeight, col)));
        }
    }
    unsigned int i = 0;
    for (int row=start;row<end-step;row+=step) {
        unsigned int j = 0;
        for (int col=start;col<end-step;col+=step) {
            const int vertexIndex = i * numberOfVertices + j;
            const int vertexIndexToRight = vertexIndex + 1;
            const int vertexIndexBelow = (i+1) * numberOfVertices + j;
            const int vertexIndexToRightBelow = vertexIndexBelow + 1;

            this->mesh.indices.push_back(vertexIndex);
            this->mesh.indices.push_back(vertexIndexToRight);
            this->mesh.indices.push_back(vertexIndexBelow);

            this->mesh.indices.push_back(vertexIndexToRight);
            this->mesh.indices.push_back(vertexIndexBelow);
            this->mesh.indices.push_back(vertexIndexToRightBelow);

            const int vertexIndexToLeft = j == 0 ? 1 : vertexIndex -1;
            const int vertexIndexUp = i == 0 ? 1 : (i-1) * numberOfVertices + j;

            const glm::vec3 norm = glm::vec3(
                    this->mesh.vertices[vertexIndexToLeft].position.y -
                    this->mesh.vertices[vertexIndexToLeft].position.y,
                    2.0f,
                    this->mesh.vertices[vertexIndexBelow].position.y -
                    this->mesh.vertices[vertexIndexUp].position.y);
            this->mesh.vertices[vertexIndex].normal = glm::normalize(norm);

            j++;
        }
        i++;
    }

    // set uniform color for now
    this->mesh.material.diffuseColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    this->initialized = true;
}

void Terrain::init() {
    this->mesh.init();

    this->terrainShader = new Shader();

    this->initialized = true;
}

void Terrain::render() {
    if (!this->initialized || this->terrainShader == nullptr) return;

    this->render(this->terrainShader);
}

void Terrain::render(Shader * shader) {
    if (!this->initialized) return;

    if (shader == nullptr) this->render();
    else {
        shader->use();
        if (shader->isBeingUsed()) {
            shader->setMat4("model", glm::mat4(1.0f));
            shader->setMat4("view", Camera::instance()->getViewMatrix());
            shader->setMat4("projection", Camera::instance()->getPerspective());
            shader->setVec3("ambientLight",  World::instance()->getAmbientLight());
            shader->setVec3("sunDirection", World::instance()->getSunDirection());
            shader->setVec3("sunLightColor", World::instance()->getSunLightColor());
            shader->setVec3("eyePosition", Camera::instance()->getPosition());
            //shader->dumpActiveShaderAttributes();
        }
    }

    this->mesh.render(shader);

    if (shader != nullptr) shader->stopUse();
}

void Terrain::cleanUp() {
    if (!this->initialized) return;

    this->mesh.cleanUp();

    this->initialized = false;
}

Terrain::~Terrain() {
    if (this->terrainShader != nullptr) delete this->terrainShader;
}
