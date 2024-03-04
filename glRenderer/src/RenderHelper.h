#pragma once

#include <glad/glad.h>
#include <memory>
#include <map>
#include <string>
#include <iostream>

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
extern const int shadowHeight;
extern const int shadowWidth;
extern int width;
extern int height;

class RenderHelper
{
public:
	RenderHelper() = default;

	void setupStaticUniforms(
		std::shared_ptr<ShaderGL> sPtr, const ShaderType sType)
	{
		sPtr->use();
		if (sType == phongLightShdr || sType == phongLightShadowShdr)
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
		else if (sType == debugQuadShdr)
		{
			sPtr->setFloat("near_plane", 1.f);
			sPtr->setFloat("far_plane", 15.f);
		}
	}

	void setupCameraUniform(
		std::shared_ptr<ShaderGL> sPtr,
		ShaderType shdrType
	)
	{
		sPtr->use();
		switch (shdrType) {
		case debugQuadShdr:
			break;
		case skyBoxShdr:
			sPtr->setMat4f("projection", camera.getPerspectiveMatrix());
			sPtr->setMat4f("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
			break;
		case singleColorShdr:
		case phongLightShadowShdr:
			sPtr->setMat4f("lightSpaceMatrix", lightSpaceMatrix);
			sPtr->setVec3f("dirLightPos", lightPos);
		case phongLightShdr:
		default:
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
		case debugQuadShdr:
			break;
		case singleColorShdr:
			sPtr->use();
			sPtr->setMat4f("model", modelMat);
			break;
		case depthShdr:
		case phongLightShadowShdr:
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
		unsigned int diffuseNr = 0;
		unsigned int specularNr = 0;
		unsigned int i = 0;

		std::string number;

		for (const auto& texturePtr : mesh.textureGLs)
		{
			const std::string& texType = texturePtr->getType();
			if (texType == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (texType == "texture_specular")
				number = std::to_string(specularNr++);
			else
				continue;

			sPtr->setInt("material." + texType + '[' + number + ']', i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texturePtr->getID());
			i++;
		}
					
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, depthMapTXT);
		sPtr->setInt("depthMap", i);
		glActiveTexture(GL_TEXTURE0);
	}

	void setupDebugDepthSampler(std::shared_ptr<ShaderGL> sPtr, const Mesh& mesh)
	{
		sPtr->setInt("depthMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTXT);
	}

	void setupSkyboxSampler(std::shared_ptr<ShaderGL> sPtr, const Mesh& mesh)
	{
		glActiveTexture(GL_TEXTURE0);
		auto texGLPtr = mesh.textureGLs[0];
		sPtr->setInt("skybox", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texGLPtr->getID());
	}

	void setupSamplers(std::shared_ptr<ShaderGL> sPtr, ShaderType shdrType, const Mesh& mesh)
	{
		sPtr->use();
		switch (shdrType) {
		case skyBoxShdr:
			setupSkyboxSampler(sPtr, mesh);
			break;
		case singleColorShdr:
		case phongLightShadowShdr:
		case phongLightShdr:
			setupPhongLightSampler(sPtr, mesh);
			break;
		case debugQuadShdr:
			setupDebugDepthSampler(sPtr, mesh);
			break;
		case depthShdr:
		default:
			break;
		}
	}

	void unsetSkyboxSampler(std::shared_ptr<ShaderGL> sPtr) const
	{
		sPtr->setInt("skybox", defaultTexUnit);
	}

	void unsetPhongLightSampler(std::shared_ptr<ShaderGL> sPtr) const
	{
		std::string number;
		for (int i = 0; i < 8; ++i)
		{
			number = std::to_string(i);
			sPtr->setInt("material.texture_diffuse[" + number + ']', defaultTexUnit);
			sPtr->setInt("material.texture_specular[" + number + ']', defaultTexUnit);
		}
		sPtr->setInt("depthMap", defaultTexUnit);
	}

	void unsetSamplers(std::shared_ptr<ShaderGL> sPtr, ShaderType shdrType) const
	{
		sPtr->use();
		switch (shdrType) {
		case skyBoxShdr:
			unsetSkyboxSampler(sPtr);
			break;
		case singleColorShdr:
			break;
		case phongLightShadowShdr:
		case phongLightShdr:
			unsetPhongLightSampler(sPtr);
			break;
		case debugQuadShdr:
		case depthShdr:
		default:
			break;
		}
	}

	void setupShadowFBO(const unsigned int width, const unsigned int height)
	{
		glGenFramebuffers(1, &depthMapFBO);
		depthMap = TextureGL2D("depth_texture", width, height, TexFilter::nearest, TexFilter::nearest,
			TexWrap::clamp_to_border, TexWrap::clamp_to_border);
		depthMapTXT = depthMap.getID();
		glBindTexture(GL_TEXTURE_2D, depthMapTXT);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTXT, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void drawObject(const ModelWithShader& mdlshdr)
	{
		auto sPtr = mdlshdr.shaderGL;
		auto sType = mdlshdr.shaderType;
		auto mPtr = mdlshdr.modelPtr;

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
			if (camera.checkBound(transformedBbox) == BoundCheckRet::Outside) {
				continue;
			}

			setupSamplers(sPtr, sType, mesh);
			setupModelUniform(sPtr, sType, trackball.getRotationMatrix() * modelMat);
			mesh.vertexGL->bind();
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
			mesh.vertexGL->unBind();
			unsetSamplers(sPtr, sType);	
		}
	}

	void drawFloor(const ModelWithShader& mdlshdr)
	{
		auto sPtr = mdlshdr.shaderGL;
		auto sType = mdlshdr.shaderType;
		auto mPtr = mdlshdr.modelPtr;

		sPtr->use();
		const glm::mat4 viewMat = camera.getViewMatrix();
		glm::mat4 modelMat(1.f);

		for (const Mesh& mesh : mPtr->meshes)
		{
			// float minY = mdlBbox.min.y;
			modelMat = glm::translate(modelMat, glm::vec3(0.f, -0.8f, 0.f));
			modelMat = glm::scale(modelMat, glm::vec3(2.f, 2.f, 2.f));
			setupSamplers(sPtr, sType, mesh);
			setupModelUniform(sPtr, sType, modelMat);
			mesh.vertexGL->bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			mesh.vertexGL->unBind();
			unsetSamplers(sPtr, sType);
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

	void drawShadowMap(const ModelWithShader& mdlshdr)
	{
		lightPos = glm::vec3(1.5f, 5.f, 1.5f);
		// lightPos = glm::vec3(-1.f, 5.f, 0.f);
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 10.5f);
		lightView = glm::lookAt(lightPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
		lightSpaceMatrix = lightProjection * lightView;
		mdlshdr.shaderGL->use();
		mdlshdr.shaderGL->setMat4f("lightSpaceMatrix", lightSpaceMatrix);

		if (mdlshdr.modelName == "floor")
			drawFloor(mdlshdr);
		else
			drawObject(mdlshdr);
	}

	void drawDebugQuad(const ModelWithShader& mdlshdr)
	{
		auto sPtr = mdlshdr.shaderGL;
		auto sType = mdlshdr.shaderType;
		auto mPtr = mdlshdr.modelPtr;

		sPtr->use();

		for (const Mesh& mesh : mPtr->meshes)
		{
			setupSamplers(sPtr, sType, mesh);
			mesh.vertexGL->bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			mesh.vertexGL->unBind();
			unsetSamplers(sPtr, sType);
		}
	}

	unsigned int getDepthMapFBO()
	{
		return depthMapFBO;
	}

private:
	unsigned int depthMapFBO;
	unsigned int depthMapTXT;
	TextureGL2D depthMap;
	glm::vec3 lightPos;
	glm::mat4 lightProjection;
	glm::mat4 lightView;
	glm::mat4 lightSpaceMatrix;
	GLint defaultTexUnit = 31; //glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexUnit);
};