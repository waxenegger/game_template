#include "render.hpp"

Model::Model(const std::string & file) {
    this->id = file;
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(file.c_str(),
            aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene == nullptr) {
        std::cerr << importer.GetErrorString() << std::endl;
        return;
    }

    if (scene->HasMeshes()) {
        for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
            const aiMesh * someMesh = scene->mMeshes[m];
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;

            for (unsigned int t = 0; t < someMesh->mNumVertices; ++t) {
                const aiVector3D someVec = someMesh->mVertices[t];
                vertices.push_back(
                        Vertex(glm::vec3(someVec.x, someVec.y, someVec.z)));
            }

            for (unsigned int t = 0; t < someMesh->mNumFaces; t++) {
                const struct aiFace face = someMesh->mFaces[t];

                for (unsigned int i = 0; i < face.mNumIndices; i++) {
                    int index = face.mIndices[i];
                    indices.push_back(index);
                }
            }

            this->meshes.push_back(Mesh(vertices, indices));
        }
        this->loaded = true;
    } else std::cerr << "Model does not contain meshes" << std::endl;
}

void Model::init() {
    if (!this->loaded) return;

    for (auto & mesh : this->meshes) mesh.init();

    this->initialized = true;
}

void Model::render() {
    if (!this->initialized) return;

    for (auto & mesh : this->meshes) mesh.render();
}

void Model::cleanUp() {
    if (!this->initialized) return;

    for (auto & mesh : this->meshes) mesh.cleanUp();

    this->initialized = false;
    this->loaded = false;
}
