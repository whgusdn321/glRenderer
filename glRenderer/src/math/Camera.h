#pragma once

#include <glm/glm.hpp>
#include "Quaternion.h"
#include "Transform.h"
#include "MathUtil.h"

extern int width, height;

struct Camera
{
	void setFOV(float inFOV);
	void setNear(float inNearZ);
	void setFar(float inFarZ);
	void setPosition(float inX, float inY, float inZ);
	void setLookAtTargetRotation(const glm::vec3 target, const glm::vec3 inUp = glm::vec3(0.f, 1.f, 0.f));

	void getViewAxes(glm::vec3&, glm::vec3&, glm::vec3& ) const;
	glm::vec3 getPosition() const;
	glm::vec3 getFrontAxis() const;
	glm::mat4 getViewMatrix() const;
	glm::mat4 getPerspectiveMatrix() const;
	glm::mat4 getPerspectiveViewMatrix() const;

	Transform transform;
	float FOV = 60.f;
	float nearZ = 5.5f;
	float farZ = 5000.f;
};

