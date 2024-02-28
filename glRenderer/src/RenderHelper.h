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
#include "MathUtil.h"
#include "ModelWithShader.h"

extern Camera camera;
extern Trackball trackball;

class RenderHelper
{
public:
	RenderHelper() = default;

	void setupStaticUniforms(
		std::shared_ptr<ShaderGL> sPtr, const ShaderType sType)
	{
		sPtr->use();
		if (sType == phongLightShdr)
		{
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

	void setupCameraUniform(
		std::shared_ptr<ShaderGL> sPtr, 
		ShaderType shdrType
	)
	{
		switch (shdrType) {
		case skyBoxShdr:
			sPtr->use();
			sPtr->setMat4f("projection", camera.getPerspectiveMatrix());
			sPtr->setMat4f("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
			break;
		case singleColorShdr:
		case phongLightShdr:
		default:
			sPtr->use();
			sPtr->setMat4f("projection", camera.getPerspectiveMatrix());
			sPtr->setVec3f("viewPos", camera.getPosition());
			sPtr->setMat4f("view", camera.getViewMatrix());
			break;
		}
	}

	void setupModelUniform(
		const std::shared_ptr<ShaderGL> sPtr,
		ShaderType shdrType,
		const glm::mat4& modelMat
	)
	{
		switch (shdrType) {
		case singleColorShdr:
			sPtr->use();
			sPtr->setMat4f("model", modelMat);
			break;
		case phongLightShdr:
			sPtr->use();
			sPtr->setMat4f("model", modelMat);
			break;
		default:
			break;
		}
	}

	void setupPhongLightSampler(std::shared_ptr<ShaderGL> sPtr, const Mesh& mesh)
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

	void unsetPhongLightSampler(std::shared_ptr<ShaderGL> sPtr, const Mesh& mesh)
	{
		for (unsigned int i = 0; i < mesh.textureGLs.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glActiveTexture(GL_TEXTURE0);
	}

	void setupSkyboxSampler(std::shared_ptr<ShaderGL> sPtr, const Mesh& mesh)
	{
		glActiveTexture(GL_TEXTURE0);
		auto texGLPtr = mesh.textureGLs[0];
		sPtr->setInt("skybox", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texGLPtr->getID());
	}

	void unsetSkyboxSampler(std::shared_ptr<ShaderGL> sPtr, const Mesh& mesh)
	{
		for (unsigned int i = 0; i < mesh.textureGLs.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glActiveTexture(GL_TEXTURE0);
	}

	void setupSamplers(std::shared_ptr<ShaderGL> sPtr, ShaderType shdrType, const Mesh& mesh)
	{
		switch (shdrType) {
		case skyBoxShdr:
			setupSkyboxSampler(sPtr, mesh);
			break;
		case singleColorShdr:
		case phongLightShdr:
			setupPhongLightSampler(sPtr, mesh);
		default:
			break;
		}
	}

	void unsetSamplers(const Mesh& mesh)
	{
		for (int i = 0; i < mesh.textureGLs.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		glActiveTexture(GL_TEXTURE0);
	}

	void drawObject(
		std::shared_ptr<ShaderGL> sPtr,
		ShaderType sType,
		const std::shared_ptr<Model> mPtr
	)
	{
		sPtr->use();
		const glm::mat4 viewMat = camera.getViewMatrix();

		for (const Mesh& mesh : mPtr->meshes)
		{
			glm::mat4 modelMat;
			
			if (sType == singleColorShdr)
			{
				glm::mat4 scaleUp = glm::mat4(1.1f);
				scaleUp[3][3] = 1.f;
				modelMat = scaleUp * mPtr->centeredTransform * mesh.transform;
			}
			else
			{
				modelMat = mPtr->centeredTransform * mesh.transform;
			}

			const BoundingBox transformedBbox = mesh.aabb.transform(viewMat * modelMat);
			if (camera.checkBound(transformedBbox) == BoundCheckRet::Outside)
				continue;

			setupSamplers(sPtr, sType, mesh);
			setupModelUniform(sPtr, sType, trackball.getRotationMatrix() * modelMat);
			mesh.vertexGL->bind();
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
			mesh.vertexGL->unBind();
			unsetSamplers(mesh);	
		}
	}

	void drawFloor(
		std::shared_ptr<ShaderGL> sPtr,
		ShaderType sType,
		const std::shared_ptr<Model> mPtr,
		const BoundingBox& mdlBbox
	)
	{
		sPtr->use();
		const glm::mat4 viewMat = camera.getViewMatrix();
		glm::mat4 modelMat(1.f);

		for (const Mesh& mesh : mPtr->meshes)
		{
			float minY = mdlBbox.min.y;
			modelMat = glm::translate(modelMat, glm::vec3(0.f, minY, 0.f));
			modelMat = glm::scale(modelMat, glm::vec3(2.f, 2.f, 2.f));
			setupSamplers(sPtr, sType, mesh);
			setupModelUniform(sPtr, sType, modelMat);
			mesh.vertexGL->bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			mesh.vertexGL->unBind();
			unsetSamplers(mesh);
		}
	}

	void drawSkybox(
		std::shared_ptr<ShaderGL> sPtr,
		ShaderType sType,
		const std::shared_ptr<Model> mPtr
	)
	{
		glDepthFunc(GL_LEQUAL);
		sPtr->use();
		const glm::mat4 viewMat = glm::mat4(glm::mat3(camera.getViewMatrix()));

		for (const Mesh& mesh : mPtr->meshes)
		{
			setupSamplers(sPtr, sType, mesh);
			mesh.vertexGL->bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			mesh.vertexGL->unBind();
		}
		glDepthFunc(GL_LESS);
	}

private:
	unsigned int depthMapFBO;
	unsigned int depthMapTXT;

};