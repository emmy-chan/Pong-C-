#pragma once

constexpr float PI = 3.14159265358979323846f;

inline void Clamp(float& i, float min, float max) {
	if (i < min)
		i = min;
	else if (i > max)
		i = max;
}

inline float RandomFloat(float a, float b)
{
	const float random = ((float)rand()) / (float)RAND_MAX;
	const float diff = b - a;
	const float r = random * diff;
	return a + r;
}

inline float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

inline float deg2rad(float degrees) {
	return degrees * (PI / 180.0f);
}

inline bool isRectInside(float rect1X, float rect1Y, float rect1W, float rect1H, float rect2X, float rect2Y, float rect2W, float rect2H) {
	// Check if any corner of Rect1 is inside Rect2
	return (rect1X < rect2X + rect2W && rect1X + rect1W > rect2X &&
		rect1Y < rect2Y + rect2H && rect1Y + rect1H > rect2Y);
}