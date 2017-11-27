#pragma once
class Matrix2
{
public:
	float mMatrix[4];
	Matrix2();
	Matrix2(float a, float b, float c, float d);
	Matrix2(float mat[4]);
	Matrix2 operator*(Matrix2&other);
	Matrix2 operator+(Matrix2&other);
	Matrix2 operator-(Matrix2&other);
};
