#pragma once
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <memory>

#include "BoundingBox.h"
#include "VertexGL.h"

class TextureGL;

enum ModelType {
    Object,
    Skybox,
};
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    ~Texture()
    {
        stbi_image_free(data);
    }
    std::string texPath;
    std::string type; // texture_diffuse/texture_specular
    int width;
    int height;
    int nrChannels;
    unsigned char* data;
};

struct Mesh {
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;

    std::shared_ptr<VertexGL> vertexGL;
    std::vector<std::shared_ptr<TextureGL>> textureGLs;

    BoundingBox aabb;
    glm::mat4 transform;
    int meshPrimitiveCnt;
};

struct Model
{
    std::string directory;
    std::string path; // relative path of the model (ex : ./model/guitar/guitar.obj

    std::vector<Mesh> meshes;
    glm::mat4 centeredTransform;
    BoundingBox rootAABB;
    unsigned int primitiveCnt;
    unsigned int vertexCnt;
    std::string modelName;

};
