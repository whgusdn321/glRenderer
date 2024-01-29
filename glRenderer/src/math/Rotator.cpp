#include "Rotator.h"

// Referenced from https://github.com/onlybooks/gamemath/blob/16-1/Source/Runtime/Math/Public/Rotator.h

void Rotator::Clamp()
{
	Yaw = GetAxisClampedValue(Yaw);
	Roll = GetAxisClampedValue(Roll);
	Pitch = GetAxisClampedValue(Pitch);
}

float Rotator::GetAxisClampedValue(float InRotatorValue)
{
	float angle = Math::fMod(InRotatorValue, 360.f);
	if (angle < 0.f)
	{
		angle += 360.f;
	}

	return angle;
}
