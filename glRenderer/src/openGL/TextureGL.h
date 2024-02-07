#pragma once

#include <glad/glad.h>
#include <string>
#include <memory>
#include <Model.h>

class TextureGL
{
public:
    TextureGL() {}
    ~TextureGL() {}
    
    GLuint getID()
    {
        return texID;
    }

    std::string getType()
    {
        return type;
    }

protected:
    GLuint texID = 0;
    std::string type;
};

class TextureGL2D : public TextureGL
{
public:
    explicit TextureGL2D(std::string _type, const int width, const int height,
        const int nrChannels, unsigned char* data)
    {
        type = _type;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    ~TextureGL2D()
    {
        glDeleteTextures(1, &texID);
    }
};

class TextureGLCubemap : public TextureGL
{
public:
    explicit TextureGLCubemap(std::shared_ptr<Texture> textures[])
    {
        type = "Cubemap";
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

        for (int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, textures[i]->width, textures[i]->height, 0, GL_RGB, GL_UNSIGNED_BYTE, textures[i]->data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    ~TextureGLCubemap()
    {
        glDeleteTextures(1, &texID);
    }
};
