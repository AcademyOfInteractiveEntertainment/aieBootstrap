#pragma once
class Snake
{
public:
	Snake();
	float mX;
	float mY;
	float SnakePos(float X, float Y);
	void changeDirection(char key); 
	float NewPos(float X, float Y);
};
