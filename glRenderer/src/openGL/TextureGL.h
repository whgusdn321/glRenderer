#pragma once

#include <glad/glad.h>
#include <string>
//#include "Model.h"

class TextureGL
{
public:
    TextureGL() {};
    explicit TextureGL(std::string _type, const int width, const int height,
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
    ~TextureGL()
    {
        glDeleteTextures(1, &texID);
    }
    
    GLuint getID()
    {
        return texID;
    }

    std::string getType()
    {
        return type;
    }
private:
    GLuint texID = 0;
    std::string type;
};
