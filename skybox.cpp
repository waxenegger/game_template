#include "render.hpp"

SkyBox::SkyBox(const std::string & dir, const std::string & texture) {
    this->dir = dir;
    this->texture = std::string(dir + "res/models/" + texture);
    this->shader = new Shader(this->dir + "/res/shaders/skybox");
}

void SkyBox::init() {
    if (this->initialized) return;

    std::vector<std::string> texNames = {
            "_right.png",
            "_left.png",
            "_top.png",
            "_bottom.png",
            "_front.png",
            "_back.png"
    };

    for (auto & t : texNames) {
        std::string f(this->texture + t);
        std::unique_ptr<Texture> tex(new Texture());
        tex->setType(Model::DIFFUSE_TEXTURE);
        tex->setPath(f);
        tex->load();
        if (tex->isValid()) textures.push_back(std::move(tex));
        else {
            std::cerr << "Failed to load skybox texture: " + f << std::endl;
            return;
        }
    }

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureId);

    int c = 0;
    for (auto & skyTex : this->textures) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + c, 0, GL_RGB,
                skyTex->getTextureSurface()->w, skyTex->getTextureSurface()->h, 0,
                skyTex->getImageFormat(), GL_UNSIGNED_BYTE, skyTex->getTextureSurface()->pixels);
        c++;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    float vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &this->skyVAO);
    glGenBuffers(1, &this->skyVBO);
    glBindVertexArray(this->skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    this->initialized = true;
}

void SkyBox::render() {
    if (!this->initialized) return;

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(this->skyVAO);

    this->shader->use();
    if (this->shader->isBeingUsed()) {
        glm::mat4 viewMatrix = Camera::instance()->getViewMatrix();
        viewMatrix[3].x = viewMatrix[3].y = viewMatrix[3].z = 0;
        this->shader->setMat4("view", viewMatrix);
        this->shader->setMat4("projection", Camera::instance()->getPerspective());
        //this->shader->setVec3("ambientLight",  World::instance()->getAmbientLight());
        //this->shader->setVec3("sunDirection", World::instance()->getSunDirection());
        //this->shader->setVec3("sunLightColor", World::instance()->getSunLightColor());
        //this->shader->setVec3("eyePosition", Camera::instance()->getPosition());
        this->shader->setInt("skybox", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureId);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        this->shader->stopUse();
    }

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void SkyBox::cleanUp() {
    if (!this->initialized) return;

    glDisableVertexAttribArray(0);
    glDeleteVertexArrays(1, &this->skyVAO);
    glDeleteBuffers(1, &this->skyVBO);
    glDeleteTextures(1, &this->textureId);

    this->initialized = false;
}
