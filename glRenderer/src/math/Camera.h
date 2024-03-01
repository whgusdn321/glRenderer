#pragma once

#include <algorithm>
#include <glm/glm.hpp>

#include "Quaternion.h"
#include "Transform.h"
#include "MathUtil.h"
#include "Frustum.h"

extern int width, height;

struct Camera
{
	Camera() = default;
	explicit Camera(float fov, float nearZ, float farZ) : FOV(fov), nearZ(nearZ), farZ(farZ) 
	{
		FOV = std::max(FOV, FOVmin);
		FOV = std::min(FOV, FOVmax);
	}
	void accumulateFOV(float ScrollChange);
	void rotateByPixels(float dy, float dx);
	void setNear(float inNearZ);
	void setFar(float inFarZ);
	void setPosition(float inX, float inY, float inZ);
	void setLookAtTargetRotation(const glm::vec3 target, const glm::vec3 inUp = glm::vec3(0.f, 1.f, 0.f));

	void getViewAxes(glm::vec3&, glm::vec3&, glm::vec3&) const;
	glm::vec3 getPosition() const;
	glm::vec3 getFrontAxis() const;
	glm::mat4 getViewMatrix() const;
	glm::mat4 getPerspectiveMatrix() const;
	glm::vec3 clip2world(glm::vec3 clipPos) const;

	BoundCheckRet checkBound(const BoundingBox& bbox);

private:
	Transform transform;
	Frustum frustum;

	const float FOVmin = 0.1f;
	const float FOVmax = 60.f;

	float FOV;
	float nearZ;
	float farZ;
};

