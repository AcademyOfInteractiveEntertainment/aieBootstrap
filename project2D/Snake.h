#pragma once
class Snake
{
public:
	Snake();
	float mX;
	float mY;
	float SnakePos(float X, float Y);
	float MoveSnake(float X, float Y);
	float NewPos(float X, float Y);
};
