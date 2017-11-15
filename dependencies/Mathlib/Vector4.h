#pragma once
#include<iostream>
class Vector4
{
public:
	float mX; float mY; float mZ;
	Vector4();
	Vector4(float x, float y, float z);
	Vector4 operator+(Vector4&other);
	Vector4 operator-(Vector4&other);
	Vector4 operator*(Vector4&other);
	bool operator==(Vector4 & other);
	float dot(Vector4&other);
	Vector4 Cross(Vector4&other);
	float Magnitude();
	Vector4 Normalize();
	float input();
	float Input();
}; 