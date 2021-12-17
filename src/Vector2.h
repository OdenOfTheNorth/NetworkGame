#pragma once
#include <iostream>
#include <math.h>

class Vector2
{
public:
	Vector2(float xValue = 0, float yValue = 0) {
		x = xValue;
		y = yValue;
	}

	float x;
	float y;

	static float lerp(float a, float b, float f) { return a + f * (b - a); }
	static float radToDeg() { return 57.2957795128961;}
	static float degToRad() { return 0.01745329252;}

	float Magnitude() { return sqrt(x * x + y * y); }

	Vector2 Lerp(Vector2 from, Vector2 to, float f)
	{
		return Vector2(lerp(from.x, to.x, f), lerp(from.y, to.y, f));
	}

	Vector2 Normalized()
	{
		float magn = Magnitude();
		return Vector2(x / magn, y / magn);
	}

	void Clamp(Vector2 min, Vector2 max) {

		if (x > max.x)
		{
			x = max.x;
		}
		if (y > max.y)
		{
			y = max.y;
		}
		if (x < min.x)
		{
			x = min.x;
		}
		if (y < min.y)
		{
			y = min.y;
		}
	}

	Vector2 operator+(const Vector2 v)
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 operator-(const Vector2 v)
	{
		return Vector2(x - v.x, y - v.y);
	}

	Vector2 operator-()
	{
		return Vector2(0 - x, 0 - y);
	}

	Vector2 operator*(const float d)
	{
		return Vector2(x * d, y * d);
	}

	Vector2 operator/(const float d)
	{
		return Vector2(x / d, y / d);
	}

	void operator+=(const Vector2& v)
	{
		this->x += v.x;
		this->y += v.y;
	}

	void operator-=(const Vector2& v)
	{
		this->x -= v.x;
		this->y -= v.y;
	}

	static float Distance(Vector2 distanceFrom, Vector2 distanceTo)
	{
		return Distance(distanceFrom.x, distanceFrom.y, distanceTo.x, distanceTo.y);
	}

	static float Distance(float distanceFromX, float distanceFromY, float distanceToX, float distanceToY)
	{
		float power = pow(distanceFromX - distanceToX, 2) + pow(distanceFromY - distanceToY, 2) * 1.0;

		return sqrt(power);
	}
};
