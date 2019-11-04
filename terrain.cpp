#include "render.hpp"

Terrain::Terrain(const std::string & dir) {
    srand (static_cast <unsigned> (time(0)));

    this->dir = dir;
    int start = -100, end = 100, step = 2;
    int numberOfVertices = (end - start) / step;
    for (int row=start;row<end;row+=step) {
        for (int col=start;col<end;col+=step) {
            const float randScaleFactor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            const float randHeight = (rand() % 4) * randScaleFactor;
            this->mesh.vertices.push_back(Vertex(glm::vec3(row, randHeight, col)));
        }
    }
    unsigned int i = 0;
    for (int row=start;row<end-step;row+=step) {
        unsigned int j = 0;
        for (int col=start;col<end-step;col+=step) {
            int vertexIndex = i * numberOfVertices + j;
            int vertexIndexToRight = vertexIndex + 1;
            int vertexIndexBelow = (i+1) * numberOfVertices + j;
            int vertexIndexToRightBelow = vertexIndexBelow + 1;

            this->mesh.indices.push_back(vertexIndex);
            this->mesh.indices.push_back(vertexIndexToRight);
            this->mesh.indices.push_back(vertexIndexBelow);

            this->mesh.indices.push_back(vertexIndexToRight);
            this->mesh.indices.push_back(vertexIndexBelow);
            this->mesh.indices.push_back(vertexIndexToRightBelow);

            const glm::vec3 edgeDownToRight = this->mesh.vertices[vertexIndexBelow].position -
                    this->mesh.vertices[vertexIndexToRightBelow].position;
            const glm::vec3 edgeDiagonallyUp = this->mesh.vertices[vertexIndexToRightBelow].position -
                    this->mesh.vertices[vertexIndex].position;
            const glm::vec3 crossA = glm::cross(edgeDiagonallyUp, edgeDownToRight);
            const glm::vec3 edgeLeft = this->mesh.vertices[vertexIndexToRight].position -
                    this->mesh.vertices[vertexIndex].position;
            const glm::vec3 crossB = glm::cross(-edgeDiagonallyUp, edgeLeft);

            this->mesh.vertices[vertexIndexBelow].normal = glm::normalize(crossA);
            this->mesh.vertices[vertexIndexToRight].normal = glm::normalize(crossB);
            const glm::vec3 normalizedAverage = glm::normalize(glm::vec3(crossA + crossB) / glm::vec3(2.0f));
            this->mesh.vertices[vertexIndex].normal = normalizedAverage;
            this->mesh.vertices[vertexIndexToRightBelow].normal = normalizedAverage;

            j++;
        }
        i++;
    }

    this->initialized = true;
}

void Terrain::init() {
    this->mesh.init();

    this->terrainShader = new Shader(std::string(this->dir + "/res/terrain"));

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
            shader->setVec4("ambientLight",  World::instance()->getAmbientLight());
            shader->setVec4("objectColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            shader->setVec3("sunDirection", World::instance()->getSunDirection());
            shader->setVec4("sunLightColor", World::instance()->getSunLightColor());
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
