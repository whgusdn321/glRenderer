#include "Camera.h"

void Camera::setFOV(float inFOV)
{
	FOV = inFOV;
}

void Camera::setNear(float inNearZ)
{
	nearZ = inNearZ; 
}

void Camera::setFar(float inFarZ) 
{
	farZ = inFarZ; 
}

void Camera::setPosition(float inX, float inY, float inZ)
{
	transform.setPosition(glm::vec3(inX, inY, inZ));
}

void Camera::setLookAtTargetRotation(const glm::vec3 target, const glm::vec3 inUp)
{
	transform.setRotation(Quaternion(target - transform.getPosition()));
}

glm::vec3 Camera::getPosition() const
{
	return transform.getPosition();
}

void Camera::getViewAxes(glm::vec3& outX, glm::vec3& outY, glm::vec3& outZ) const
{
	outX = -transform.getLocalX();
	outY = transform.getLocalY();
	outZ = -transform.getLocalZ();
}

glm::vec3 Camera::getFrontAxis() const
{
	return transform.getLocalZ();
}

glm::mat4 Camera::getViewMatrix() const
{
	glm::vec3 viewX, viewY, viewZ;
	getViewAxes(viewX, viewY, viewZ);
	glm::vec3 pos = transform.getPosition();

	return glm::mat4(
		glm::vec4(viewX.x, viewY.x, viewZ.x, 0.f),
		glm::vec4(viewX.y, viewY.y, viewZ.y, 0.f),
		glm::vec4(viewX.z, viewY.z, viewZ.z, 0.f),
		glm::vec4(-glm::dot(viewX, pos), -glm::dot(viewY, pos), -glm::dot(viewZ, pos), 1.f)
	);
}

glm::mat4 Camera::getPerspectiveMatrix() const
{
	float invA = 1.f / ((float)width / height);
	float d = 1.f / tanf(Math::deg2Rad(FOV) * 0.5f);

	float invNF = 1.f / (nearZ - farZ);
	float k = (farZ + nearZ) * invNF;
	float l = 2.f * farZ * nearZ * invNF;
	return glm::mat4(
		glm::vec4(d * invA, 0.f, 0.f, 0.f),
		glm::vec4(0.f, d, 0.f, 0.f),
		glm::vec4(0.f, 0.f, k, -1.f),
		glm::vec4(0.f, 0.f, l, 0.f)
	);
}
