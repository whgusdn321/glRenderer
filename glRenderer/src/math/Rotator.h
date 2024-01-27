#pragma once

#include "MathUtil.h"

struct Rotator
{
public:
	constexpr Rotator() = default;
	constexpr Rotator(float inYaw, float inRoll, float inPitch) : Yaw(inYaw), Roll(inRoll), Pitch(inPitch) {}
	void Clamp();
	float GetAxisClampedValue(float InRotatorValue);

public:
	float Yaw = 0.f;
	float Roll = 0.f;
	float Pitch = 0.f;
};

