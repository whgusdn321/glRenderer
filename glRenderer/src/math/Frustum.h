#pragma once

#include "Plane.h"
#include "MathUtil.h"
#include "BoundingBox.h"

#include <glm/gtc/matrix_access.hpp>

enum BoundCheckRet
{
	Inside = 0,
	Intersect = 1,
	Outside = 2,
};

struct Frustum
{
	Frustum(const glm::mat4& perspectiveMat)
	{
		update(perspectiveMat);
	}

	void update(const glm::mat4 perspectiveMat)
	{
		planes[0] = Plane(-(glm::row(perspectiveMat, 3) - glm::row(perspectiveMat, 1)));
		planes[1] = Plane(-(glm::row(perspectiveMat, 3) + glm::row(perspectiveMat, 1)));
		planes[2] = Plane(-(glm::row(perspectiveMat, 3) - glm::row(perspectiveMat, 0)));
		planes[3] = Plane(-(glm::row(perspectiveMat, 3) + glm::row(perspectiveMat, 0)));
		planes[4] = Plane(-(glm::row(perspectiveMat, 3) - glm::row(perspectiveMat, 2)));
		planes[5] = Plane(-(glm::row(perspectiveMat, 3) + glm::row(perspectiveMat, 2)));
	}

	constexpr BoundCheckRet checkBound(const BoundingBox& bbox) const
	{
		for (auto& plane : planes)
		{
			glm::vec3 innerBoxPoint(bbox.max); // assume normal element is negative.
			glm::vec3 outerBoxPoint(bbox.min);

			if (plane.getNx() >= 0.f)
			{
				innerBoxPoint.x = bbox.min.x;
				outerBoxPoint.x = bbox.max.x;
			}
			if (plane.getNy() >= 0.f)
			{
				innerBoxPoint.y = bbox.min.y;
				outerBoxPoint.y = bbox.max.y;
			}
			if (plane.getNz() >= 0.f)
			{
				innerBoxPoint.z = bbox.min.z;;
				outerBoxPoint.z = bbox.max.z;
			}

			if (plane.isOutside(innerBoxPoint))
			{
				return BoundCheckRet::Outside;
			}
			if (plane.isOutside(outerBoxPoint) && !plane.isOutside(innerBoxPoint))
			{
				return BoundCheckRet::Intersect;
			}
		}
		return BoundCheckRet::Inside;
	}
private:
	Plane planes[6];
};
