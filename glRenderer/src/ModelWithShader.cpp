#include "ModelWithShader.h"
ModelLoader ModelWithShader::modelLoader;
std::map<std::string, std::shared_ptr<TextureGL>> ModelWithShader::textureGLCache; // key : texture path
std::map<ShaderType, std::shared_ptr<ShaderGL>> ModelWithShader::shaderGLCache;