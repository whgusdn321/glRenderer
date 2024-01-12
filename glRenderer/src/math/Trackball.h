#pragma once

#include <iostream>
#include <algorithm>

extern int width;
extern int height;

struct Trackball
{
	Trackball() = default;
	Trackball(const Camera& camera) : camera(camera) {}

	glm::mat4 getRotationMatrix(bool isTrackballOn, const float startX,
								const float startY, const float endX, const float endY);

private:
	void rotate(const float startX, const float startY, const float endX, const float endY);
	float getZ(float x, float y) const;
	glm::vec3 get_trackball_vector(float x, float y);
private:
	float r = 100.f;
	Quaternion quaternion;
	const Camera& camera;
	Transform transform;

};

void Trackball::rotate(const float startX, const float startY, const float endX, const float endY)
{
	glm::vec3 va = get_trackball_vector(startX, startY);
	glm::vec3 vb = get_trackball_vector(endX, endY);
	float angle = acos(std::min(1.f, glm::dot(va, vb)));
	glm::vec3 axisInCameraCoord = glm::cross(va, vb);
	glm::mat3 camera2object = glm::inverse(glm::mat3(camera.getViewMatrix()) * glm::mat3(1.f));
	glm::vec3 axisInObjectCoord = camera2object * axisInCameraCoord;
	Quaternion qtr(axisInObjectCoord, Math::rad2Deg(angle));
	quaternion.accumulate(qtr);
}

glm::vec3 Trackball::get_trackball_vector(float x, float y) 
{
	glm::vec3 P = glm::vec3(
		( x / width * 2) - 1.0,
		( y / height * 2) - 1.0,
		0
	);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;
	if (OP_squared <= 1 * 1)
		P.z = sqrt(1 * 1 - OP_squared);  // Pythagoras
	else
		P = glm::normalize(P);  // nearest point
	return P;
}

glm::mat4 Trackball::getRotationMatrix(bool isTrackballOn, const float startX,
									   const float startY, const float endX, const float endY)
{
	transform.setRotation(quaternion);

	if (isTrackballOn && !(Math::equalsInTolerance(startX, endX) && Math::equalsInTolerance(startY, endY)))
		rotate(startX, startY, endX, endY);

	return transform.getModelingMatrix();
}

float Trackball::getZ(float x, float y) const
{
	float rr = r * r;
	float xx = x * x;
	float yy = y * y;
	if (xx + yy <= rr * 0.5f)
	{
		return sqrtf(rr - (xx + yy));
	}
	float div = xx + yy;
	return (rr * 0.5f) / sqrtf(div);
}
