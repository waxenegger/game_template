#include "render.hpp"

void Mesh::init() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    int i=0;
    for (auto & texture : this->textures) {
        const std::string textureLocation(texture.getPath());

        SDL_Surface * textureSurface = IMG_Load(textureLocation.c_str());
        if(textureSurface != nullptr) {
            GLenum texture_format;
            const GLint nOfColors = textureSurface->format->BytesPerPixel;

            if (nOfColors == 4) {
               if (textureSurface->format->Rmask == 0x000000ff) texture_format = GL_RGBA;
               else texture_format = GL_BGRA;
            } else if (nOfColors == 3) {
               if (textureSurface->format->Rmask == 0x000000ff) texture_format = GL_RGB;
               else texture_format = GL_BGR;
            } else {
                std::cerr << "Unsupported Texture Format: " << textureLocation << std::endl;
                SDL_FreeSurface(textureSurface);
                continue;
            }

            GLuint id = 0;
            glGenTextures(1, &id);
            texture.setId(id);

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture.getId());

            glTexImage2D(GL_TEXTURE_2D, 0, texture_format, textureSurface->w, textureSurface->h, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, textureSurface->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
            SDL_FreeSurface(textureSurface);
            i++;
        } else std::cerr << "Failed to load texture: " << textureLocation << std::endl;
    }

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
}

void Mesh::render(Shader * shader) {
    glBindVertexArray(this->VAO);

    if (shader != nullptr && shader->isBeingUsed()) {
        shader->setInt(Model::AMBIENT_TEXTURE, 0);
        shader->setInt(Model::DIFFUSE_TEXTURE, 0);
        shader->setInt(Model::SPECULAR_TEXTURE, 0);
        shader->setInt(Model::TEXTURE_NORMALS, 0);

        int i=0;
        for (auto & texture : this->textures) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture.getId());

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

            shader->setInt(texture.getType(), i);
            shader->setInt("has_" + texture.getType(), 1);

            i++;
        }
    }

    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::cleanUp() {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
    for (auto & texture : this->textures) texture.cleanUp();
}
