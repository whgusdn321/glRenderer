#include "Quaternion.h"

Quaternion::Quaternion(const glm::vec3& inAxis, float inAngleDegree)
{
	const glm::vec3 normalizedAxis = glm::normalize(inAxis);
	float sin = 0.f, cos = 0.f;
	Math::getSinCos(sin, cos, inAngleDegree * 0.5f);
	w = cos;
	x = sin * normalizedAxis.x;
	y = sin * normalizedAxis.y;
	z = sin * normalizedAxis.z;
}

Quaternion::Quaternion(const glm::mat3& inMatrix)
{
	float root = 0.f;
	float trace = inMatrix[0][0] + inMatrix[1][1] + inMatrix[2][2];

	if (!Math::equalsInTolerance(glm::length(inMatrix[0]), 1.f) || !Math::equalsInTolerance(glm::length(inMatrix[1]), 1.f) || Math::equalsInTolerance(glm::length(inMatrix[2]), 1.f))
	{
		*this = Quaternion(0.f, 0.f, 0.f, 1.f);
	}

	if (trace > 0.f)
	{
		root = sqrtf(trace + 1.f);
		w = 0.5f * root;
		root = 0.5f / root;

		x = (inMatrix[1][2] - inMatrix[2][1]) * root;
		y = (inMatrix[2][0] - inMatrix[0][2]) * root;
		z = (inMatrix[0][1] - inMatrix[1][0]) * root;
	}
	else
	{
		char i = 0;

		if (inMatrix[1][1] > inMatrix[0][0]) { i = 1; }
		if (inMatrix[2][2] > inMatrix[i][i]) { i = 2; }

		static const char next[3] = { 1, 2, 0 };
		const char j = next[i];
		const char k = next[j];

		root = sqrtf(inMatrix[i][i] - inMatrix[j][j] - inMatrix[k][k] + 1.f);

		float* qt[3] = { &x, &y, &z };
		*qt[i] = 0.5f * root;

		root = 0.5f / root;

		*qt[j] = (inMatrix[i][j] + inMatrix[j][i]) * root;
		*qt[k] = (inMatrix[i][k] + inMatrix[k][i]) * root;

		w = (inMatrix[j][k] - inMatrix[k][j]) * root;
	}

}

Quaternion::Quaternion(const glm::vec3& inVector, const glm::vec3& inUp)
{
	glm::vec3 zAxis = glm::normalize(inVector);
	glm::vec3 xAxis;
	glm::vec3 yAxis;
	if (abs(zAxis.y) >= (1.f - Math::SMALL_NUMBER))
	{
		xAxis = { 1.f, 0.f, 0.f };
	}
	else
	{
		xAxis = glm::normalize(glm::cross(inUp, zAxis));
	}
	yAxis = glm::normalize(glm::cross(zAxis, xAxis));
	*this = Quaternion(glm::mat3(xAxis, yAxis, zAxis));
}

Quaternion& Quaternion::accumulate(const Quaternion& inQuaternion)
{
	glm::vec3 v1(x, y, z), v2(inQuaternion.x, inQuaternion.y, inQuaternion.z);
	glm::vec3 v = v2 * w + v1 * inQuaternion.w + glm::cross(v2, v1);
	w = w * inQuaternion.w - glm::dot(v1, v2);
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

Quaternion Quaternion::operator*=(const Quaternion& inQuaternion)
{
	glm::vec3 v1(x, y, z), v2(inQuaternion.x, inQuaternion.y, inQuaternion.z);
	glm::vec3 v = v2 * w + v1 * inQuaternion.w + glm::cross(v1, v2);
	w = w * inQuaternion.w - glm::dot(v1, v2);
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

Rotator Quaternion::toRotator() const
{
	Rotator result;
	float sinrCosp = 2 * (w * z + x * y);
	float cosrCosp = 1 - 2 * (z * z + x * x);
	result.Roll = Math::rad2Deg(atan2f(sinrCosp, cosrCosp));

	float pitchTest = w * x - y * z;
	float asinThreshold = 0.4999995f;
	float sinp = 2 * pitchTest;
	if (pitchTest < -asinThreshold)
	{
		result.Pitch = -90.f;
	}
	else if (pitchTest > asinThreshold)
	{
		result.Pitch = 90.f;
	}
	else
	{
		result.Pitch = Math::rad2Deg(asinf(sinp));
	}

	float sinyCosp = 2 * (w * y + x * z);
	float cosyCosp = 1.f - 2 * (x * x + y * y);
	result.Yaw = Math::rad2Deg(atan2f(sinyCosp, cosyCosp));

	return result;
}
