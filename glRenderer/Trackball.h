#pragma once

#include <iostream>

extern int width;
extern int height;

struct Trackball
{
	void rotate();
	glm::mat4 getRotationMatrix() const;
	float getZ(float x, float y) const;
	// below x, y are screen coordinates
	float startX;
	float startY;
	float endX;
	float endY;
	float objectX = 0.f;
	float objectY = 0.f;

	float r=100.f;

	Quaternion quaternion;
};
void Trackball::rotate()
{
	const float x1 = startX - width / 2.f;
	const float y1 = height / 2.f - startY;
	const float z1 = getZ(x1, y1);
	std::cout << "x1: " << x1 << "\ty1: " << y1 << "\tz1: " << z1 << std::endl;
	const float x2 = endX - width / 2.f;
	const float y2 = height / 2.f - endY;
	const float z2 = getZ(x2, y2);
	std::cout << "x2: " << x2 << "\ty2: " << y2 << "\tz2: " << z2 << std::endl;

	glm::vec3 v1(x1, y1, z1);
	glm::vec3 v2(x2, y2, z2);
	v1 = glm::normalize(v1);
	v2 = glm::normalize(v2);
	std::cout << "v1: " << v1.x << "\t" << v1.y << "\t" << v1.z << std::endl;
	std::cout << "v2: " << v2.x << "\t" << v2.y << "\t" << v2.z << std::endl;

	glm::vec3 n = glm::cross(v1, v2);
	std::cout << "n: " << n.x << "\t" << n.y << "\t" << n.z << std::endl;
	float theta = acos(glm::dot(v1, v2)); // radian
	std::cout << "theta: " << theta << std::endl;

	Quaternion q_tmp(n, Math::rad2Deg(theta));
	quaternion *= q_tmp;
}

glm::mat4 Trackball::getRotationMatrix() const
{
	/*
	const float x1 = startX - width / 2.f;
	const float y1 = height/2.f - startY;
	const float z1 = getZ(x1, y1);
	std::cout << "x1: " << x1 << "\ty1: " << y1 << "\tz1: " << z1 << std::endl;
	const float x2 = endX - width / 2.f;
	const float y2 = height / 2.f - endY;
	const float z2 = getZ(x2, y2);
	std::cout << "x2: " << x2 << "\ty2: " << y2 << "\tz2: " << z2 << std::endl;
	
	glm::vec3 v1(x1, y1, z1);
	glm::vec3 v2(x2, y2, z2);
	v1 = glm::normalize(v1);
	v2 = glm::normalize(v2);
	std::cout << "v1: " << v1.x << "\t" << v1.y << "\t" << v1.z << std::endl;
	std::cout << "v2: " << v2.x << "\t" << v2.y << "\t" << v2.z << std::endl;

	glm::vec3 n = glm::cross(v1, v2);
	std::cout << "n: " << n.x << "\t" << n.y << "\t" << n.z << std::endl;
	float theta = acos(glm::dot(v1, v2)); // radian
	std::cout << "theta: " << theta << std::endl;
	
	Quaternion q_tmp(n, Math::rad2Deg(theta));
	*/
	Transform tmp(quaternion);
	return tmp.getModelingMatrix();
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
	// if (div < 1.e-4f) {
	// 	div += 1.e-2f;
	// }
	return (rr * 0.5f) / sqrtf(div);
}
