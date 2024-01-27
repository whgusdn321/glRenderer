#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "Model.h"
#include "TextureGL.h"

class ModelLoader
{
public:
    std::shared_ptr<Model> loadModel(std::string modelName);

private:
    std::string directory;
    std::map<std::string, std::shared_ptr<Model>> modelCache; // key: modelName
    std::map<std::string, std::shared_ptr<Texture>> textureCache; // key: texture path (ex:./model/guitar/normal.png )

private:
    void processNode(std::shared_ptr<Model> model, aiNode* aiNode, const aiScene* aiScene, glm::mat4& transform);
    Mesh processMesh(aiMesh* aiMesh, const aiScene* aiScene);

    std::shared_ptr<Texture> textureFromFile(std::string texPath);
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);

    glm::mat4 convertMatrix(const aiMatrix4x4& m);
    glm::mat4 adjustModelCenter(const BoundingBox& bbox);
};
