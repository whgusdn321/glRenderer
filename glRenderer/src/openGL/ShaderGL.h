#pragma once

#include <iostream>
#include <string>
#include <glm/gtc/type_ptr.hpp>

class ShaderGL
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    ShaderGL(std::string vertexPath, std::string fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3f(const std::string& name, glm::vec3 vec) const;
    void setVec3f(const std::string& name, float x, float y, float z) const;
    void setMat4f(const std::string& name, glm::mat4 matrix) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
