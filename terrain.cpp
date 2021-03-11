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
    float u=0.0f;
    float v=0.0f;
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
            this->mesh.indices.push_back(vertexIndexToRightBelow);
            this->mesh.indices.push_back(vertexIndexBelow);

            const int vertexIndexToLeft = j == 0 ? 1 : vertexIndex -1;
            const int vertexIndexUp = i == 0 ? 1 : (i-1) * numberOfVertices + j;

            const glm::vec3 norm = glm::vec3(
                    this->mesh.vertices[vertexIndexToLeft].position.y -
                    this->mesh.vertices[vertexIndexToRight].position.y,
                    2.0f,
                    this->mesh.vertices[vertexIndexBelow].position.y -
                    this->mesh.vertices[vertexIndexUp].position.y);
            this->mesh.vertices[vertexIndex].normal = glm::normalize(norm);

            this->mesh.vertices[vertexIndex].uv = glm::vec2(u, v);
            v += 0.005;
            if (v > 1.0) v = 0.0f;
            j++;
        }
        u += 0.005;
        if (u > 1.0) u = 0.0f;
        i++;
    }
}

void Terrain::init() {
    if (this->initialized) return;

    this->useShader(new Shader(this->dir + "/res/shaders/terrain"));

    std::vector<std::string> texNames = {
            "/res/models/grass.png"
    };

    for (auto & t : texNames) {
        std::string f(this->dir + t);
        std::unique_ptr<Texture> tex(new Texture());
        tex->setType(Model::DIFFUSE_TEXTURE);
        tex->setPath(f);
        tex->load();
        if (tex->isValid()) this->textures.push_back(std::move(tex));
        else {
            std::cerr << "Failed to load terrain texture: " + f << std::endl;
            return;
        }
    }

    glGenTextures(1, &this->textureId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->textureId);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLfloat color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    int c = 0;
    for (auto & tex : this->textures) {
        GLenum imageFormat;
           if (!Texture::findImageFormat(tex->getTextureSurface(), &imageFormat)) {
               std::cerr << "Unsupported Image Format: " << std::endl;
               return;
           }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->getTextureSurface()->w, tex->getTextureSurface()->h, 0,
                        imageFormat, GL_UNSIGNED_BYTE, tex->getTextureSurface()->pixels);
        c++;
    }

    /*glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA, 100, 100, this->textures.size());

    int c = 0;
    for (auto & tex : this->textures) {
        std::cout << tex->getTextureSurface()->w << "x" << tex->getTextureSurface()->w << std::endl;
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                0, 0, 0, c,
                tex->getTextureSurface()->w, tex->getTextureSurface()->h, 1,
                GL_RGBA, GL_UNSIGNED_BYTE, tex->getTextureSurface()->pixels);
        c++;
    }*/

    this->mesh.init();

    this->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    std::vector<Material> materials;
    materials.push_back(this->getMaterial());
    this->setMaterials(materials);

    this->setPosition(glm::vec3(0.0f));
    std::vector<glm::mat4> modelMatrices;
    modelMatrices.push_back(this->calculateTransformationMatrix());
    this->setModelMatrices(modelMatrices);

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
        if (this->textures.size() > 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->textureId);
            this->shader->setInt("has_texture", 1);
            this->shader->setInt("textureArray", 0);
        } else this->shader->setInt("has_texture", 0);

        this->mesh.render(this->shader);

        this->shader->stopUse();
    }
}

void Terrain::setMaterials(std::vector<Material> & materials) {
    this->mesh.setMaterials(materials);
}

void Terrain::setModelMatrices(std::vector<glm::mat4> & modelMatrices) {
    this->mesh.setModelMatrices(modelMatrices);
}

void Terrain::cleanUp() {
    if (!this->initialized) return;

    glDeleteTextures(1, &this->textureId);
    this->mesh.cleanUp();

    this->initialized = false;
}
