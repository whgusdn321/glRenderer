#pragma once

#include <glm/glm.hpp>
#include "Quaternion.h"
#include "Transform.h"
#include "MathUtil.h"
#include "Frustum.h"

extern int width, height;

struct Camera
{
	Camera(float fov, float nearZ, float farZ) : FOV(fov), nearZ(nearZ), farZ(farZ) {}
	void setPosition(float inX, float inY, float inZ);
	void setLookAtTargetRotation(const glm::vec3 target, const glm::vec3 inUp = glm::vec3(0.f, 1.f, 0.f));

	void getViewAxes(glm::vec3&, glm::vec3&, glm::vec3&) const;
	glm::vec3 getPosition() const;
	glm::vec3 getFrontAxis() const;
	glm::mat4 getViewMatrix() const;
	glm::mat4 getPerspectiveMatrix() const;

	float FOV;
	float nearZ;
	float farZ;

private:
	Transform transform;
};

