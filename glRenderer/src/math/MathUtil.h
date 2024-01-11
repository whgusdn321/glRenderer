#pragma once

struct Math
{
	static constexpr float PI = { 3.14159265358979323846f };
	static constexpr float HalfPI = { 1.57079632679f };
	static constexpr float InvPI = { 0.31830988618f };
	static constexpr float SMALL_NUMBER = 1.e-8f;

	static float fMod(float X, float Y)
	{
		if (fabsf(Y) <= SMALL_NUMBER)
		{
			return 0.f;
		}

		const float quotient = (float)(int)(X / Y);
		float intPortion = Y * quotient;
		if (fabsf(intPortion) > fabsf(X))
		{
			intPortion = X;
		}

		return (X - intPortion);
	}

	static constexpr bool equalsInTolerance(float inFloat1, float inFloat2, float inTolerance = 1.e-8f)
	{
		return abs(inFloat1 - inFloat2) <= inTolerance;
	}


	static constexpr void getSinCosRad(float& outSin, float& outCos, float inRadian)
	{
		// Copied from UE4 Source Code
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float quotient = (InvPI * 0.5f) * inRadian;
		if (inRadian >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}
		float y = inRadian - (2.0f * PI) * quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign = 0.f;
		if (y > HalfPI)
		{
			y = PI - y;
			sign = -1.0f;
		}
		else if (y < -HalfPI)
		{
			y = -PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y * y;

		// 11-degree minimax approximation
		outSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		outCos = sign * p;
	}

	static constexpr void getSinCos(float& outSin, float& outCos, float inDegree)
	{
		if (inDegree == 0.f)
		{
			outSin = 0.f;
			outCos = 1.f;
		}
		else if (inDegree == 90.f)
		{
			outSin = 1.f;
			outCos = 0.f;
		}
		else if (inDegree == 180.f)
		{
			outSin = 0.f;
			outCos = -1.f;
		}
		else if (inDegree == 270.f)
		{
			outSin = -1.f;
			outCos = 0.f;
		}
		else
		{
			getSinCosRad(outSin, outCos, deg2Rad(inDegree));
		}
	}

	static constexpr float deg2Rad(float d)
	{
		return d / 180.f * PI;
	}

	static constexpr float rad2Deg(float d)
	{
		return d * 180.f * InvPI;
	}


	template<class T>
	static constexpr T abs(const T& A)
	{
		return (A >= (T)0) ? A : -A;
	}



};
