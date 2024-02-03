#pragma once

#include <string>
#include <memory>

#include "Model.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "Trackball.h"
#include "ShaderGL.h"
#include "Frustum.h"

enum ShaderType {
	PhongLight = 0,
	SingleColor,
};

struct RenderResources 
{
    RenderResources(ShaderType shdrType, std::string modelName = "rodin")
    {
		setModel(modelName);
		setShader(shdrType);
    }
	
	void setModel(std::string modelName)
	{
		this->modelName = modelName;
		modelPtr = modelLoader.loadModel(modelName);
		setupVertexGL(modelPtr);
		setupTextureGL(modelPtr);
	}

	void setShader(ShaderType shdrType)
	{
		this->shaderType = shdrType;
		shaderGL = setupShaderGL(shaderType);
	}

	void setupVertexGL(std::shared_ptr<Model> model)
	{
		if (!model->meshes.empty() && !(model->meshes[0].vertexGL))
		{
			for (Mesh& mesh : model->meshes)
			{
				mesh.vertexGL = std::make_shared<VertexGL>(&mesh.vertices[0], &mesh.indices[0],
					mesh.vertices.size(), mesh.indices.size(),
					offsetof(Vertex, normal), offsetof(Vertex, texCoords),
					sizeof(Vertex));
			}
		}
	}

	void setupTextureGL(std::shared_ptr<Model> model)
	{
		if (!model->meshes.empty() && model->meshes[0].textureGLs.empty())
		{
			for (Mesh& mesh : model->meshes)
			{
				for (std::shared_ptr<Texture> tx : mesh.textures)
				{
					if (textureGLCache.find(tx->texPath) == textureGLCache.end())
					{
						std::shared_ptr<TextureGL> outTx = std::make_shared<TextureGL>(
							tx->type, tx->width, tx->height, tx->nrChannels, tx->data
							);
						textureGLCache[tx->texPath] = outTx;
					}
					mesh.textureGLs.push_back(textureGLCache[tx->texPath]);
				}
			}
		}
	}

	std::shared_ptr<ShaderGL> setupShaderGL(const ShaderType shaderType)
	{
		if (shaderGLCache.find(shaderType) == shaderGLCache.end())
		{
			auto shaderPtr = createShaderGL(shaderType);
			shaderGLCache[shaderType] = shaderPtr;
		}
		return shaderGLCache[shaderType];
	}

	std::shared_ptr<ShaderGL> createShaderGL(const ShaderType shaderType)
	{
		auto sdrPaths = getShaderPath(shaderType);
		std::string vsPath = sdrPaths.first;
		std::string fsPath = sdrPaths.second;

		auto shaderPtr = std::make_shared<ShaderGL>(vsPath, fsPath);
		return shaderPtr;
	}

	std::pair<std::string, std::string> getShaderPath(ShaderType shaderType)
	{
		std::string vsPath, fsPath;
		switch (shaderType)
		{
		case SingleColor:
			vsPath = "./shaders/singlecolor.vs";
			fsPath = "./shaders/singlecolor.fs";
			break;
		case PhongLight:
		default:
			vsPath = "./shaders/phong_light.vs";
			fsPath = "./shaders/phong_light.fs";
			break;
		}
		return std::make_pair(vsPath, fsPath);
	}

    
public:
    static bool frustumCulling;
    static bool highlightBoundary;

    // model
    std::shared_ptr<Model> modelPtr;
    std::string modelName;

    //shader
    ShaderType shaderType;
    std::shared_ptr<ShaderGL> shaderGL;



private:
    static ModelLoader modelLoader;

	static std::map<std::string, std::shared_ptr<TextureGL>> textureGLCache; // key : texture path
	static std::map<ShaderType, std::shared_ptr<ShaderGL>> shaderGLCache; 

};
