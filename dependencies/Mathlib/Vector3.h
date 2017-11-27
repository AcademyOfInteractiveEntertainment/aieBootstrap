#pragma once
#include<iostream>
class Vector3 
{
public:
	float mX; float mY; float mZ;
	Vector3(); 
	Vector3(float x, float y,float z);
	Vector3 operator+(Vector3&other);
	Vector3 operator-(Vector3&other);
	Vector3 operator*(Vector3&other);
	bool operator==(Vector3 & other);
	float dot(Vector3&other);
	Vector3 Cross(Vector3&other);
	float Magnitude();
	Vector3 Normalize(); 
	float Input();
	float input(); 

}; 