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

    Mesh m;

    for (int row=start;row<end;row+=step) {
        for (int col=start;col<end;col+=step) {
            const float randHeight = static_cast<const float>((rand() % 4));
            m.vertices.push_back(Vertex(glm::vec3(row, randHeight, col)));
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

            m.indices.push_back(vertexIndex);
            m.indices.push_back(vertexIndexToRight);
            m.indices.push_back(vertexIndexBelow);

            m.indices.push_back(vertexIndexToRight);
            m.indices.push_back(vertexIndexToRightBelow);
            m.indices.push_back(vertexIndexBelow);

            const int vertexIndexToLeft = j == 0 ? 1 : vertexIndex -1;
            const int vertexIndexUp = i == 0 ? 1 : (i-1) * numberOfVertices + j;

            const glm::vec3 norm = glm::vec3(
                    m.vertices[vertexIndexToLeft].position.y -
                    m.vertices[vertexIndexToRight].position.y,
                    2.0f,
                    m.vertices[vertexIndexBelow].position.y -
                    m.vertices[vertexIndexUp].position.y);
            m.vertices[vertexIndex].normal = glm::normalize(norm);

            j++;
        }
        i++;
    }

    this->mesh.push_back(m);

    // set uniform color for now
    this->setColor(0.0f, 1.0f, 0.0f, 1.0f);

    this->initialized = true;
}

void Terrain::init() {
    if (this->mesh.size() != 0) this->mesh[0].init();

    this->initialized = true;
}

void Terrain::render() {
    if (!this->initialized || this->shader == nullptr) return;

    this->shader->use();
    if (this->shader->isBeingUsed()) {
        this->shader->setMat4("view", Camera::instance()->getViewMatrix());
        this->shader->setMat4("projection", Camera::instance()->getPerspective());
        this->shader->setVec3("ambientLight",  World::instance()->getAmbientLight());
        this->shader->setVec3("sunDirection", World::instance()->getSunDirection());
        this->shader->setVec3("sunLightColor", World::instance()->getSunLightColor());
        this->shader->setVec3("eyePosition", Camera::instance()->getPosition());
        //shader->dumpActiveShaderAttributes();

        if (this->mesh.size() != 0) this->mesh[0].render(this->shader);

        this->shader->stopUse();
    }
}

void Terrain::cleanUp() {
    if (!this->initialized) return;

    if (this->mesh.size() != 0) this->mesh[0].cleanUp();

    this->initialized = false;
}
