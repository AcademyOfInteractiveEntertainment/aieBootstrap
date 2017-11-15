#pragma once
#include<iostream>
class Vector2
{
public:
	float mX;
	float mY;
	Vector2();
	Vector2(float X, float Y);
	Vector2 operator+(Vector2&other);
	Vector2 operator-(Vector2&other);
	Vector2 operator*(Vector2&other);
	Vector2 Normalise();
	float Magnitude();
	bool  operator==(Vector2&other);
	float Input();
	float input();
};
 