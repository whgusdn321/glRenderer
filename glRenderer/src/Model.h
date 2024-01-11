#pragma once
#include <string>
#include <vector>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"

class Model
{
public:
    Model(std::string path)
    {
        loadModel(path);
    }
    void draw(Shader& shader);
private:
    // model data
    std::vector<Mesh> meshes;
    std::map<std::string, Texture> path2texture;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);
};
