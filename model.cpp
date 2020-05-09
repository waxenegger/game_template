#include "render.hpp"

Model::Model(const std::string & dir, const std::string & file) {
    this->file = std::string(dir + file);
    this->dir = dir;
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(this->file.c_str(),
            aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

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

         this->addTextures(material, aiTextureType_AMBIENT, Model::AMBIENT_TEXTURE, textures);
         this->addTextures(material, aiTextureType_DIFFUSE, Model::DIFFUSE_TEXTURE, textures);
         this->addTextures(material, aiTextureType_SPECULAR, Model::SPECULAR_TEXTURE, textures);
         this->addTextures(material, aiTextureType_HEIGHT, Model::TEXTURE_NORMALS, textures);
     }

     if (mesh->mNumVertices > 0) vertices.reserve(mesh->mNumVertices);
     for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
         Vertex vertex(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

         if (mesh->HasNormals())
             vertex.normal = glm::normalize(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));

         if(mesh->HasTextureCoords(0)) {
             vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
         } else vertex.uv = glm::vec2(0.0f, 0.0f);

         if (mesh->HasTangentsAndBitangents()) {
             if (mesh->mTangents->Length() == mesh->mNumVertices)
                 vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

             if (mesh->mBitangents->Length() == mesh->mNumVertices)
                 vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
         }

         if (ambient != nullptr) vertex.colorAmbient = glm::vec3(ambient->r, ambient->g, ambient->b);
         if (diffuse != nullptr) vertex.colorDiffuse = glm::vec3(diffuse->r, diffuse->g, diffuse->b);
         if (specular != nullptr) vertex.colorSpecular = glm::vec3(specular->r, specular->g, specular->b);

         vertices.push_back(vertex);
     }

     for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
         const aiFace face = mesh->mFaces[i];

         for(unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
     }

     return Mesh(vertices, indices, textures);
}

void Model::correctTexturePath(char * path) {
    int index = 0;

    while(path[index] == '\0') index++;

    if(index != 0) {
        int i = 0;
        while(path[i + index] != '\0') {
            path[i] = path[i + index];
            i++;
        }
        path[i] = '\0';
    }
}

void Model::addTextures(const aiMaterial * mat, const aiTextureType type, const std::string name, std::vector<Texture> & textures) {
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        if (str.length > 0) this->correctTexturePath(str.data);

        const std::string fullyQualifiedName(this->dir + "/res/models/" + std::string(str.C_Str(), str.length));

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

    if (shader != nullptr && shader->isBeingUsed()) {
        shader->setMat4("view", Camera::instance()->getViewMatrix());
        shader->setMat4("projection", Camera::instance()->getPerspective());
        shader->setVec3("ambientLight",  World::instance()->getAmbientLight());
        shader->setVec3("sunDirection", World::instance()->getSunDirection());
        shader->setVec3("sunLightColor", World::instance()->getSunLightColor());
        shader->setVec3("eyePosition", Camera::instance()->getPosition());
    }

    for (auto & mesh : this->meshes) mesh.render(shader);
}

void Model::cleanUp() {
    if (!this->initialized) return;

    for (auto & mesh : this->meshes) mesh.cleanUp();

    this->initialized = false;
    this->loaded = false;
}

void Model::useMaterial(const Material & material) {
    for (auto & mesh : this->meshes) mesh.material = material;
}

void Model::useNormalsTexture(const bool flag) {
    for (auto & mesh : this->meshes) mesh.useNormalsTexture = flag;
}

const std::string Model::AMBIENT_TEXTURE = "texture_ambient";
const std::string Model::DIFFUSE_TEXTURE = "texture_diffuse";
const std::string Model::SPECULAR_TEXTURE = "texture_specular";
const std::string Model::TEXTURE_NORMALS = "texture_normals";
