#include <iostream>
#include <stb_image.h>
#include <memory>
#include <glm/gtx/string_cast.hpp>

#include "Model.h"
#include "ModelLoader.h"
#include "VertexGL.h"
#include "TextureGL.h"

std::shared_ptr<Model> ModelLoader::loadModel(ModelLoadType modelType, std::string modelName)
{
    if (modelCache.find(modelName) != modelCache.end())
    {
		return modelCache[modelName];
    }
    
    std::shared_ptr<Model> model;
    if (modelType == Object)
        model = loadObjectModel(modelName);
    else if (modelType == Skybox)
        model = loadSkyboxModel(modelName);
    else if (modelType == Floor)
        model = loadFloorModel(modelName);
    else if (modelType == DebugQuad)
        model = loadDebugQuad(modelName);

    modelCache[modelName] = model;
    return model;
}

std::shared_ptr<Model> ModelLoader::loadObjectModel(std::string modelName)
{
    std::shared_ptr<Model> model = std::make_shared<Model>();
    model->directory = "./model/" + modelName + '/';
    model->path = model->directory + modelName + ".obj";
    this->directory = model->directory;

    Assimp::Importer importer;

    //todo: modelName -> path + modelname
    const aiScene* scene = importer.ReadFile(model->path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return model;
    }

    glm::mat4 curTransform = glm::mat4(1.f);
    processNode(model, scene->mRootNode, scene, curTransform);

    model->centeredTransform = adjustModelCenter(model->rootAABB);
    return model;
}

std::shared_ptr<Model> ModelLoader::loadDebugQuad(std::string modelName)
{
    std::shared_ptr<Model> model = std::make_shared<Model>();
    model->primitiveCnt = 2;
    model->vertexCnt = 4;

    Mesh mesh;
    mesh.meshPrimitiveCnt = 2;

    float quadVertices[] = {
        // positions                // texcoords
         -1.0f, -1.0f, 0.f,   0.0f,  0.0f, // 7'o clock
        1.0f, 1.0f, 0.0f,    1.0f,  1.0f, // 1 
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // 5 

         -1.0f, -1.0f, 0.0f,   0.0f,  0.0f, // 7 
        1.0f, 1.0f, 0.0f,     1.0f, 1.0f, // 1 
         -1.0f, 1.0f, 0.0f,   0.0f, 1.0f // 11
    };

    for (int i = 0; i < 6; ++i)
    {
        Vertex v;
        v.position.x = quadVertices[i * 5];
        v.position.y = quadVertices[i * 5 + 1];
        v.position.z = quadVertices[i * 5 + 2];
        v.texCoords.s = quadVertices[i * 5 + 3];
        v.texCoords.t = quadVertices[i * 5 + 4];
        mesh.vertices.push_back(v);
    }

    model->meshes.push_back(mesh);
    model->modelName = "quad";
    return model;
}

std::shared_ptr<Model> ModelLoader::loadFloorModel(std::string modelName)
{
    std::shared_ptr<Model> model = std::make_shared<Model>();
    model->primitiveCnt = 2;
    model->vertexCnt = 4;

    Mesh mesh;
    mesh.meshPrimitiveCnt = 2;

    auto dir = "./model_floor/";
    model->directory = dir;
    this->directory = dir;

    const std::string filePath = dir + modelName + ".png";
    std::shared_ptr<Texture> loadedTexture = textureFromFile(filePath);
    loadedTexture->type = "texture_diffuse";
    mesh.textures.push_back(std::move(loadedTexture));

    float planeVertices[] = {
        // positions            // normals         // texcoords
         1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f, // 5'o clock
        -1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f, // 7
        -1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // 11

         1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f, // 5
        -1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // 11
         1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f // 1
    };

    for (int i = 0; i < 6; ++i)
    {
        Vertex v;
        v.position.x = planeVertices[i * 8];
        v.position.y = planeVertices[i * 8 + 1];
        v.position.z = planeVertices[i * 8 + 2];
        v.normal.x = planeVertices[i * 8 + 3];
        v.normal.y = planeVertices[i * 8 + 4];
        v.normal.z = planeVertices[i * 8 + 5];
        v.texCoords.s = planeVertices[i * 8 + 6];
        v.texCoords.t = planeVertices[i * 8 + 7];
        mesh.vertices.push_back(v);
    }
    mesh.transform = glm::mat4(1.f);
    mesh.aabb = BoundingBox(glm::vec3(-1.0f, -0.5f, -1.f), glm::vec3(1.0f, -0.5f, 1.f));
    model->centeredTransform = glm::mat4(1.f);

    model->meshes.push_back(mesh);
    model->modelName = "floor";
    return model;
}

std::shared_ptr<Model> ModelLoader::loadSkyboxModel(std::string modelName)
{
    std::shared_ptr<Model> model = std::make_shared<Model>();
    model->primitiveCnt = 12;
    model->vertexCnt = 36;

    Mesh mesh;
    mesh.meshPrimitiveCnt = 12;

    auto dir = "./model_skybox/" + modelName + '/';
    model->directory = dir;
    this->directory = dir;

    // set mesh's texture
    const std::string filePaths[6] = {dir + "right.jpg", dir + "left.jpg", dir + "top.jpg", dir + "bottom.jpg", dir + "front.jpg", dir + "back.jpg"};

    std::shared_ptr<Texture> loadedTextures[6];
    int i = 0;
    for (const auto& filepath : filePaths)
    {
        loadedTextures[i++] = textureFromFile(filepath);
    }
    std::shared_ptr<TextureGL> textureGL = std::make_shared<TextureGLCubemap>(loadedTextures);
    mesh.textureGLs.push_back(textureGL);

    // set mesh's vertex
    float skyboxVertices[] = {
        // positions          
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

    for (int i = 0; i < 12; ++i) // # triangle
    {
        for (int j = 0; j < 3; ++j) // # vert
        {
            Vertex v;
            v.position.x = skyboxVertices[i*9 + j*3 + 0];
            v.position.y = skyboxVertices[i*9 + j*3 + 1];
            v.position.z = skyboxVertices[i*9 + j*3 + 2];
            mesh.vertices.push_back(v);
        }
    }
    model->meshes.push_back(mesh);
    return model;
}

void ModelLoader::processNode(std::shared_ptr<Model> model, aiNode* aiNode, const aiScene* aiScene, glm::mat4& transform)
{
    glm::mat4 nodeTransform = convertMatrix(aiNode->mTransformation);
    glm::mat4 currTransform = transform * nodeTransform;

    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < aiNode->mNumMeshes; i++)
    {
        aiMesh* mesh = aiScene->mMeshes[aiNode->mMeshes[i]];
        Mesh ourMesh = processMesh(mesh, aiScene);
        ourMesh.transform = currTransform;

        BoundingBox bounds = ourMesh.aabb.transform(currTransform);
        model->rootAABB.merge(bounds);
       
        model->primitiveCnt += ourMesh.meshPrimitiveCnt;
        model->vertexCnt += ourMesh.vertices.size();
        model->meshes.push_back(std::move(ourMesh));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < aiNode->mNumChildren; i++)
    {
        processNode(model, aiNode->mChildren[i], aiScene, currTransform);
    }
}

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* aiScene)
{
    Mesh outMesh;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        }
        else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process material, especially texture
    if (mesh->mMaterialIndex >= 0) // check if mesh has material or not 
    {
        aiMaterial* material = aiScene->mMaterials[mesh->mMaterialIndex];
        auto diffuseMaps = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        auto specularMaps = loadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    }

    // process bounding box
    const aiVector3D& aiBoxMin = mesh->mAABB.mMin;
    const aiVector3D& aiBoxMax = mesh->mAABB.mMax;
    const glm::vec3& bboxMin = glm::vec3(aiBoxMin.x, aiBoxMin.y, aiBoxMin.z);
    const glm::vec3& bboxMax = glm::vec3(aiBoxMax.x, aiBoxMax.y, aiBoxMax.z);

    BoundingBox bbox(bboxMin, bboxMax);

    outMesh.vertices = std::move(vertices);
    outMesh.indices = std::move(indices);
    outMesh.textures = std::move(textures);
    outMesh.aabb = bbox;

    outMesh.meshPrimitiveCnt = mesh->mNumFaces;
   
    return outMesh;
}

std::vector<std::shared_ptr<Texture>> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
    std::string typeName)
{
    std::vector<std::shared_ptr<Texture>> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str); // get texture file name
        std::string texFileName = std::string(str.C_Str());
        std::string texPath = directory + texFileName;
        
        if (textureCache.find(texPath) == textureCache.end()) {
            std::shared_ptr<Texture> texture = textureFromFile(texPath); // texture loading
            texture->type = typeName;
            textureCache[texPath] = texture;
        }
        textures.push_back(textureCache[texPath]);
    }
    return textures;
}

std::shared_ptr<Texture> ModelLoader::textureFromFile(std::string texPath)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        const char* failureReason = stbi_failure_reason();
        std::cout << "During load [" << texPath << "], ";
        std::cout << "Failed to load image: " << failureReason << std::endl;
    }

    std::shared_ptr<Texture> outTexture = std::make_shared<Texture>();
    outTexture->data = data;
    outTexture->width = width;
    outTexture->height = height;
    outTexture->nrChannels = nrChannels;
    outTexture->texPath = texPath;
    return outTexture;
}


glm::mat4 ModelLoader::convertMatrix(const aiMatrix4x4& m)
{
    glm::mat4 ret;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ret[j][i] = m[i][j];
        }
    }
    return ret;
}

glm::mat4 ModelLoader::adjustModelCenter(const BoundingBox& bbox)
{
    glm::mat4 modelTransform(1.0f);
    glm::vec3 trans = (bbox.max + bbox.min) / -2.f;
    // trans.y = -bbox.min.y;
    float bounds_len = glm::length(bbox.max - bbox.min);
    modelTransform = glm::scale(modelTransform, glm::vec3(3.f / bounds_len));
    modelTransform = glm::translate(modelTransform, trans);
    return modelTransform;
}