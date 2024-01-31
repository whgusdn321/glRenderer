#pragma once

#include "MathUtil.h"

// Referenced from https://github.com/onlybooks/gamemath/blob/16-1/Source/Runtime/Math/Public/Rotator.h

struct Rotator
{
public:
	constexpr Rotator() = default;
	constexpr Rotator(/*degree*/float inYaw, float inRoll, float inPitch) : Yaw(inYaw), Roll(inRoll), Pitch(inPitch) {}
	void Clamp();
	float GetAxisClampedValue(float InRotatorValue);

public:
	float Yaw = 0.f; /*degree*/
	float Roll = 0.f;
	float Pitch = 0.f;
};

