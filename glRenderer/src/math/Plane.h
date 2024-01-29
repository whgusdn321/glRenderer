#pragma once

#include <glm/glm.hpp>
#include "MathUtil.h"

struct Plane
{
	constexpr Plane() = default;
	explicit Plane(float nx, float ny, float nz, float d) : nx(nx), ny(ny), nz(nz), d(d) { normalize(); }
	explicit Plane(glm::vec3 norm, float d) : nx(norm.x), ny(norm.y), nz(norm.z), d(d) { normalize(); }
	explicit Plane(glm::vec4 v) : nx(v.x), ny(v.y), nz(v.z), d(v.w) { normalize(); }
	constexpr float distance(glm::vec3 point) const
	{
		return point.x * nx + point.y * ny + point.z * nz + d;
	}
	constexpr bool isOutside(glm::vec3 point) const 
	{
		return distance(point) > 0.f;
	}
	constexpr float getNx() const {
		return nx;
	}
	constexpr float getNy() const {
		return ny;
	}
	constexpr float getNz() const {
		return nz;
	}
	
private:
	void normalize()
	{
		float size = sqrt(nx * nx + ny * ny + nz * nz);
		if (Math::equalsInTolerance(size, 1.f))
			return;
		nx /= size;
		ny /= size;
		nz /= size;
		d /= size;
	}
private:
	float nx = 0.f;
	float ny = 0.f;
	float nz = 0.f;
	float d = 0.f;
};
