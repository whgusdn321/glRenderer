#pragma once

#include <glad/glad.h>
#include <string>
#include <memory>
#include <Model.h>

enum TexFilter
{
    nearest = GL_NEAREST,
    linear = GL_LINEAR,
    nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
    nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
    linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST,
    linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR,
};

enum TexWrap
{
    repeat = GL_REPEAT,
    clamp_to_edge = GL_CLAMP_TO_EDGE,
    clamp_to_border = GL_CLAMP_TO_BORDER,
    mirrored_repeat = GL_MIRRORED_REPEAT
};

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
    TextureGL2D() {}
    explicit TextureGL2D(std::shared_ptr<Texture> texture, 
        TexFilter magFilter,
        TexFilter minFilter,
        TexWrap texWrapS,
        TexWrap texWrapT)
    {
        type = texture->type;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);


        GLenum format = GL_RGB;
        if (texture->nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);

        if (!(minFilter == TexFilter::nearest || minFilter == TexFilter::linear))
            glGenerateMipmap(GL_TEXTURE_2D);
    }

    explicit TextureGL2D(std::string type,
        unsigned int width,
        unsigned int height,
        TexFilter magFilter,
        TexFilter minFilter,
        TexWrap texWrapS,
        TexWrap texWrapT)
    {
        this->type = type;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    }

    ~TextureGL2D()
    {
        //glDeleteTextures(1, &texID);
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
