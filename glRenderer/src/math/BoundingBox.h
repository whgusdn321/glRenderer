#pragma once

#include <glm/glm.hpp>

struct BoundingBox
{
public:
	BoundingBox() = default;
	BoundingBox(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

	void getCorners(glm::vec3* dst) const;
	BoundingBox transform(const glm::mat4& matrix) const;
	void merge(const BoundingBox& box);
	bool intersects(const BoundingBox& box);

private:
	static void updateMinMax(glm::vec3* point, glm::vec3* min, glm::vec3* max);

public:
	glm::vec3 min{ 0.f, 0.f, 0.f };
	glm::vec3 max{ 0.f, 0.f, 0.f };
};
