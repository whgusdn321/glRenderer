#pragma once

#include <glm/glm.hpp>
#include "MathUtil.h"
#include "Rotator.h"

// Referenced from https://github.com/onlybooks/gamemath/blob/16-1/Source/Runtime/Math/Public/Quaternion.h

struct Quaternion
{
public:
	constexpr Quaternion() = default;
	explicit constexpr Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	explicit Quaternion(const glm::vec3& inAxis, float inAngleDegree);
	explicit constexpr Quaternion(const Rotator& inRotator)	
	{
		float sp = 0.f, sy = 0.f, sr = 0.f;
		float cp = 0.f, cy = 0.f, cr = 0.f;

		Math::getSinCos(sp, cp, inRotator.Pitch * 0.5f);
		Math::getSinCos(sy, cy, inRotator.Yaw * 0.5f);
		Math::getSinCos(sr, cr, inRotator.Roll * 0.5f);

		w = sy * sp * sr + cy * cp * cr;
		x = sy * sr * cp + sp * cy * cr;
		y = sy * cp * cr - sp * sr * cy;
		z = -sy * sp * cr + sr * cy * cp;

	}
	explicit Quaternion(const glm::mat3& inMatrix);
	explicit Quaternion(const glm::vec3& inVector, const glm::vec3& inUp = glm::vec3(0.f, 1.f, 0.f));
	Quaternion& accumulate(const Quaternion& inQuaternion);

	Quaternion operator*=(const Quaternion& inQuaternion);
	constexpr glm::vec3 operator*(const glm::vec3& inVector) const
	{
		// rotate vector with quaternion
		glm::vec3 q(x, y, z);
		glm::vec3 t = glm::cross(q, inVector) * 2.f;
		return inVector + t * w + glm::cross(q, t);
	}

	static Quaternion slerp(const Quaternion& inQuaternion1, const Quaternion& inQuaternion2, float inRatio);
	Rotator toRotator() const;

	constexpr void normalize();
	

	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 1.f;
};
