#pragma once
struct Rotator
{
public:
	constexpr Rotator() = default;
	constexpr Rotator(float inYaw, float inRoll, float inPitch) : Yaw(inYaw), Roll(inRoll), Pitch(inPitch) { }
	void Clamp()
	{
		Yaw = GetAxisClampedValue(Yaw);
		Roll = GetAxisClampedValue(Roll);
		Pitch = GetAxisClampedValue(Pitch);
	}

	float GetAxisClampedValue(float InRotatorValue)
	{
		float angle = Math::fMod(InRotatorValue, 360.f);
		if (angle < 0.f)
		{
			angle += 360.f;
		}

		return angle;
	}


public:
	float Yaw = 0.f;
	float Roll = 0.f;
	float Pitch = 0.f;
};

