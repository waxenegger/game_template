#include "render.hpp"

Model::Model(const std::string & dir, const std::string & file) {
    this->file = std::string(dir + file);
    this->dir = dir;
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(this->file.c_str(),
            aiProcess_Triangulate | aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene == nullptr) {
        std::cerr << importer.GetErrorString() << std::endl;
        return;
    }

    if (scene->HasMeshes()) {
        this->processNode(scene->mRootNode, scene);
        this->loaded = true;
    } else std::cerr << "Model does not contain meshes" << std::endl;
}

void Model::processNode(const aiNode * node, const aiScene *scene) {
    for(unsigned int i=0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }

    for(unsigned int i=0; i<node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
     std::vector<Vertex> vertices;
     std::vector<unsigned int> indices;
     std::vector<Texture> textures;

     std::unique_ptr<aiColor4D> ambient(new aiColor4D());
     std::unique_ptr<aiColor4D> diffuse(new aiColor4D());
     std::unique_ptr<aiColor4D> specular(new aiColor4D());

     if (scene->HasMaterials()) {
         const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

         aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, ambient.get());
         aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, diffuse.get());
         aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, specular.get());

         this->addTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures);
         this->addTextures(material, aiTextureType_SPECULAR, "texture_specular", textures);
         this->addTextures(material, aiTextureType_HEIGHT, "texture_normal", textures);
         this->addTextures(material, aiTextureType_AMBIENT, "texture_height", textures);
     }

     for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
         Vertex vertex(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

         if (mesh->HasNormals() && mesh->mNormals->Length() == mesh->mNumVertices)
             vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

         if(mesh->HasTextureCoords(0)) {
             vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
         } else vertex.uv = glm::vec2(0.0f, 0.0f);

         if (mesh->HasTangentsAndBitangents()) {
             if (mesh->mTangents->Length() == mesh->mNumVertices)
                 vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

             if (mesh->mBitangents->Length() == mesh->mNumVertices)
                 vertex.bitTangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
         }

         if (ambient != nullptr) vertex.colorAmbient = glm::vec3(ambient->r, ambient->g, ambient->b);
         if (diffuse != nullptr) vertex.colorDiffuse = glm::vec3(diffuse->r, diffuse->g, diffuse->b);
         if (specular != nullptr) vertex.colorSpecular = glm::vec3(specular->r, specular->g, specular->b);

         vertices.push_back(vertex);
     }

     for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
         const aiFace face = mesh->mFaces[i];
         for(unsigned int j = 0; j < face.mNumIndices; j++)
             indices.push_back(face.mIndices[j]);
     }

     return Mesh(vertices, indices, textures);
}

void Model::addTextures(const aiMaterial * mat, const aiTextureType type, const std::string name, std::vector<Texture> & textures) {
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        const std::string fullyQualifiedName(this->dir + std::string(str.C_Str()));

        std::map<std::string, Texture>::iterator val(TEXTURES.find(fullyQualifiedName));

        if (val != TEXTURES.end()) textures.push_back(val->second);
        else {
            Texture texture;
            texture.setType(name);
            texture.setPath(fullyQualifiedName);
            textures.push_back(texture);
            TEXTURES[str.C_Str()] = texture;
        }
    }
}

void Model::init() {
    if (!this->loaded) return;

    for (auto & mesh : this->meshes) mesh.init();

    this->initialized = true;
}

void Model::render(Shader * shader) {
    if (!this->initialized) return;

    for (auto & mesh : this->meshes) mesh.render(shader);
}

void Model::cleanUp() {
    if (!this->initialized) return;

    for (auto & mesh : this->meshes) mesh.cleanUp();

    this->initialized = false;
    this->loaded = false;
}
