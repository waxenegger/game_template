#include "render.hpp"
#include "game.hpp"

Image * Image::fromFile(std::string file) {
    Image * img = new Image();
    img->id = file;

    std::map<std::string, std::shared_ptr<Texture>>::iterator val(Game::TEXTURES.find(file));

    if (val == Game::TEXTURES.end()) {
        std::shared_ptr<Texture> texture(new Texture());
        texture->setType(Model::DIFFUSE_TEXTURE);
        texture->setPath(file);
        texture->load();
        if (texture->isValid()) Game::TEXTURES[file] = std::move(texture);
        else {
            texture.reset();
            return img;
        }
    }

    if (Game::TEXTURES[file]->isValid()) {
        img->image = Game::TEXTURES[file]->getTextureSurface();
        img->init();
    }

    return img;
}

Image * Image::fromText(std::string fontFile, std::string text, int size) {
    Image * img = new Image();

    if (text.size() == 0) return img;

    img->text = text;

    TTF_Font * font = TTF_OpenFont(fontFile.c_str(), size);

    if (font != nullptr) {
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
        const SDL_Color bg = { 255, 255, 255 };
        const SDL_Color fg = { 0, 0, 0 };

        SDL_Surface * tmp = TTF_RenderUTF8_Shaded(font, text.c_str(), fg, bg);
        if (tmp != nullptr) {

            SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
            img->image = SDL_ConvertSurface(tmp, format, 0);

            SDL_FreeFormat(format);
            SDL_FreeSurface(tmp);

            img->init();
        }

        TTF_CloseFont(font);
    }

    return img;
}

void Image::init() {
    if (this->image == nullptr) return;

    GLenum imageFormat;
    if (!Texture::findImageFormat(this->image, &imageFormat)) {
        std::cerr << "Unsupported Image Format: " << std::endl;
        return;
    }

    const float zDirNormal = -1.0f;
    const float w = this->image->w;
    const float h = this->image->h;

    Vertex one(glm::vec3(0.0f, h, 0.0f));
    one.uv = glm::vec2(1.0f, 0.0f);
    one.normal = glm::vec3(0.0f, h, zDirNormal);
    this->mesh.vertices.push_back(one);

    Vertex two(glm::vec3(w, h, 0.0f));
    two.uv = glm::vec2(0.0f, 0.0f);
    two.normal = glm::vec3(w, h, zDirNormal);
    this->mesh.vertices.push_back(two);

    Vertex three(glm::vec3(w, 0.0f, 0.0f));
    three.uv = glm::vec2(0.0f, 1.0f);
    three.normal = glm::vec3(w, 0.0f, zDirNormal);
    this->mesh.vertices.push_back(three);

    Vertex four(glm::vec3(0.0f, 0.0f, 0.0f));
    four.uv = glm::vec2(1.0f, 1.0f);
    four.normal = glm::vec3(0.0f, 0.0f, zDirNormal);
    this->mesh.vertices.push_back(four);

    this->mesh.indices.push_back(2);
    this->mesh.indices.push_back(3);
    this->mesh.indices.push_back(0);

    this->mesh.indices.push_back(1);
    this->mesh.indices.push_back(2);
    this->mesh.indices.push_back(0);

    glGenTextures(1, &this->textureId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->textureId);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->image->w, this->image->h, 0,
                    imageFormat, GL_UNSIGNED_BYTE, this->image->pixels);
    //glGenerateMipmap(GL_TEXTURE_2D);

    this->mesh.init();

    this->initialized = true;
}

void Image::render() {
    if (this->shader == nullptr) return;

    this->shader->use();
    if (this->shader->isBeingUsed()) {

        this->shader->setMat4("view", Camera::instance()->getViewMatrix());
        this->shader->setMat4("projection", Camera::instance()->getPerspective());

        this->shader->setVec3("ambientLight",  World::instance()->getAmbientLight());
        this->shader->setVec3("sunDirection", World::instance()->getSunDirection());
        this->shader->setVec3("sunLightColor", World::instance()->getSunLightColor());
        this->shader->setVec3("eyePosition", Camera::instance()->getPosition());

        this->shader->setInt("has_" + Model::AMBIENT_TEXTURE, 0);
        this->shader->setInt("has_" + Model::SPECULAR_TEXTURE, 0);
        this->shader->setInt("has_" + Model::TEXTURE_NORMALS, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureId);
        this->shader->setInt("has_" + Model::DIFFUSE_TEXTURE, 1);
        this->shader->setInt(Model::DIFFUSE_TEXTURE, 0);

        this->mesh.render(this->shader);

        this->shader->stopUse();
    }
}

void Image::setMaterials(std::vector<Material> & materials) {
    this->mesh.setMaterials(materials);
}

void Image::setModelMatrices(std::vector<glm::mat4> & modelMatrices) {
    this->mesh.setModelMatrices(modelMatrices);
}

void Image::cleanUp() {
    glDeleteTextures(1, &this->textureId);
    this->mesh.cleanUp();
}

Image::~Image() {
    if (this->text.size() != 0 && this->image != nullptr) {
        SDL_FreeSurface(this->image);
        this->image = nullptr;
    }
}
