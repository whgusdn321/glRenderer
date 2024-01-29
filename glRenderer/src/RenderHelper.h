#pragma once

#include <glad/glad.h>
#include <memory>
#include <map>
#include <string>

#include "Model.h"
#include "VertexGL.h"
#include "TextureGL.h"
#include "ShaderGL.h"
#include "Camera.h"
#include "Trackball.h"
#include "Frustum.h"

extern bool isTrackballOn;

enum ShaderType {
	PhongLight = 0,
};

class RenderHelper
{
public:
	RenderHelper() = default;

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

	void setupStaticUniforms(
		std::shared_ptr<ShaderGL> sPtr, const ShaderType sType, Camera& camera)
	{
		sPtr->use();
		if (sType == PhongLight)
		{
			sPtr->setMat4f("projection", camera.getPerspectiveMatrix());
			sPtr->setVec3f("viewPos", camera.getPosition());

			sPtr->setFloat("material.shiniess", 32.0f);

			sPtr->setVec3f("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
			sPtr->setVec3f("directionalLight.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
			sPtr->setVec3f("directionalLight.specular", 1.0f, 1.0f, 1.0f);
			sPtr->setVec3f("directionalLight.direction", -0.2f, -1.0f, -0.3f);

			sPtr->setVec3f("pointLights[0].position", 3.f, 3.f, 3.f);
			sPtr->setVec3f("pointLights[1].position", -3.f, -3.f, 3.f);
			sPtr->setVec3f("pointLights[2].position", -3.f, 3.f, -3.f);
			sPtr->setVec3f("pointLights[3].position", 3.f, -3.f, -3.f);
			for (int pl = 0; pl < 4; ++pl) {
				sPtr->setVec3f("pointLights[" + std::to_string(pl) + "].ambient", 0.1f, 0.1f, 0.1f);
				sPtr->setVec3f("pointLights[" + std::to_string(pl) + "].diffuse", 0.2f, 0.2f, 0.2f);
				sPtr->setVec3f("pointLights[" + std::to_string(pl) + "].specular", 1.f, 1.f, 1.f);
				sPtr->setFloat("pointLights[" + std::to_string(pl) + "].constant", 1.f);
				sPtr->setFloat("pointLights[" + std::to_string(pl) + "].linear", 0.07f);
				sPtr->setFloat("pointLights[" + std::to_string(pl) + "].quadratic", 0.017f);
			}
			sPtr->setVec3f("flashLight.position", camera.getPosition());
			sPtr->setVec3f("flashLight.direction", camera.getFrontAxis());
			sPtr->setFloat("flashLight.cutOff", glm::cos(glm::radians(12.5f)));
			sPtr->setFloat("flashLight.outerCutOff", glm::cos(glm::radians(17.5f)));
			sPtr->setVec3f("flashLight.ambient", 0.1f, 0.1f, 0.1f);
			sPtr->setVec3f("flashLight.flash", 0.3f, 0.3f, 0.3f);
			
		}
	}

	void setupDynamicUniforms(
		std::shared_ptr<ShaderGL> sPtr,
		const std::shared_ptr<Model> mPtr,
		const Camera& camera,
		const Trackball& trackball
	)
	{
		sPtr->use();
		sPtr->setMat4f("view", camera.getViewMatrix());
	}

	void setupModelUniform(
		const std::shared_ptr<ShaderGL> sPtr,
		const glm::mat4& modelMat,
		const glm::mat4& rotationByMouse
	)
	{
		sPtr->use();
		sPtr->setMat4f("model", rotationByMouse * modelMat);
	}
	
	void drawFrame(
		std::shared_ptr<ShaderGL> sPtr,
		const std::shared_ptr<Model> mPtr,
		const Trackball& trackball,
		Frustum& frustum,
		const Camera& camera
	)
	{
		// update frustum with camera's current perspective matrix
		frustum.update(camera.getPerspectiveMatrix());

		for (const Mesh& mesh : mPtr->meshes)
		{
			const glm::mat4 modelMat = mPtr->centeredTransform * mesh.transform;

			const BoundingBox transformedBbox = mesh.aabb.transform(modelMat);
			if (frustum.checkBound(transformedBbox) == BoundCheckRet::Outside)
				continue;

			setupSamplers(sPtr, mesh);
			setupModelUniform(sPtr, modelMat, trackball.getRotationMatrix());
			mesh.vertexGL->bind();
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
			mesh.vertexGL->unBind();
		}
	}

	void setupSamplers(std::shared_ptr<ShaderGL> sPtr, const Mesh& mesh)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (unsigned int i = 0; i < mesh.textureGLs.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			auto texGLPtr = mesh.textureGLs[i];
			std::string texType = texGLPtr->getType();
			std::string number;

			if (texType == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (texType == "texture_specular")
				number = std::to_string(specularNr++);

			sPtr->setInt("material." + texType + '[' + number + ']', i);

			glBindTexture(GL_TEXTURE_2D, texGLPtr->getID());
		}
		glActiveTexture(GL_TEXTURE0);
	}
	
	
	std::pair<std::string, std::string> getShaderPath(ShaderType shaderType)
	{
		std::string vsPath, fsPath;
		switch (shaderType)
		{
			case PhongLight:
			default:
				vsPath = "./shaders/phong_light.vs";
				fsPath = "./shaders/phong_light.fs";
				break;
		}
		return std::make_pair(vsPath, fsPath);
	}
private:
	std::map<std::string, std::shared_ptr<TextureGL>> textureGLCache; // key : texture path
	std::map<ShaderType, std::shared_ptr<ShaderGL>> shaderGLCache; 
};