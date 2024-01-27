#pragma once

#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>

#include "Quaternion.h"
#include "Camera.h"
#include "Transform.h"

extern int width;
extern int height;
extern bool isTrackballOn;

struct Trackball
{
	Trackball() = default;
	Trackball(const Camera& camera) : camera(camera) {}

	glm::mat4 getRotationMatrix() const;
	void rotate(const float startX, const float startY, const float endX, const float endY);

private:
	glm::vec3 get_trackball_vector(float x, float y) const;
	float getZ(float x, float y) const;
private:
	float r = 100.f;
	Quaternion quaternion;
	const Camera& camera;
	Transform transform;

};