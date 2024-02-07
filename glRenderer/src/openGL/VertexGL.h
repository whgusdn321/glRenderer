#pragma once

#include <glad/glad.h>

class VertexGL
{
public:
    VertexGL() {};
    explicit VertexGL(const void* vertices, const void* indices, unsigned int vertexCnt,
                        unsigned int indexCnt, std::size_t normalOffset, std::size_t texOffset, std::size_t vertexStructSize)
	{
        glGenVertexArrays(1, &vaoID);

        glBindVertexArray(vaoID);
        if (vertices)
        {
            glGenBuffers(1, &vboID);
            glBindBuffer(GL_ARRAY_BUFFER, vboID);
            glBufferData(GL_ARRAY_BUFFER, vertexCnt * vertexStructSize, vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStructSize, (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexStructSize, (void*)normalOffset);

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexStructSize, (void*)texOffset);
        }

        if (indices)
        {
            glGenBuffers(1, &eboID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCnt * sizeof(unsigned int), indices, GL_STATIC_DRAW);
        }

    }

    ~VertexGL()
    {
        if (vboID)
            glDeleteBuffers(1, &vboID);
        if (vaoID)
            glDeleteBuffers(1, &vaoID);
        if (eboID)
            glDeleteBuffers(1, &eboID);
    }
    
    int getID() const
    {
        return vaoID;
    }

    void bind() const
    {
        if (vaoID)
            glBindVertexArray(vaoID);
    }

    void unBind() const
    {
        glBindVertexArray(0);
    }
private:
	GLuint vaoID = 0;
	GLuint vboID = 0;
	GLuint eboID = 0;
};
