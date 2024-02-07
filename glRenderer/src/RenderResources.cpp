#include "RenderResources.h"
ModelLoader RenderResources::modelLoader;
std::map<std::string, std::shared_ptr<TextureGL>> RenderResources::textureGLCache; // key : texture path
std::map<ShaderType, std::shared_ptr<ShaderGL>> RenderResources::shaderGLCache;