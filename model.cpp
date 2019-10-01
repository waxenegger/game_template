#include "render.hpp"

Model::Model(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    this->vertices = vertices;
    this->indices = indices;
    this->init();
}

void Model::init() {
    // create buffers/arrays
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
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
}

void Model::render() {
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
}

void Model::cleanUp() {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

Model * Model::loadModelFromObjectFile(const std::string & file) {
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(file.c_str(),
            aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene == nullptr) {
        std::cerr << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    if (scene->HasMeshes()) {
        for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
            const aiMesh * someMesh = scene->mMeshes[m];
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indexes;

            for (unsigned int t = 0; t < someMesh->mNumVertices; ++t) {
                const aiVector3D someVec = someMesh->mVertices[t];
                vertices.push_back(
                        Vertex(glm::vec3(someVec.x, someVec.y, someVec.z)));
            }

            for (unsigned int t = 0; t < someMesh->mNumFaces; t++) {
                const struct aiFace face = someMesh->mFaces[t];

                for (unsigned int i = 0; i < face.mNumIndices; i++) {
                    int index = face.mIndices[i];
                    indexes.push_back(index);
                }
            }
            return new Model(vertices, indexes);
        }
    }

    return nullptr;
}
