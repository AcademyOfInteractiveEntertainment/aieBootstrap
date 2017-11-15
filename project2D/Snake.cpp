#include "Snake.h"

Snake::Snake()
{
}

float Snake::SnakePos(float X, float Y)
{
	mX = X;
	mY = Y;
	return 0;
}

float Snake::MoveSnake(float X, float Y)
{
	Snake S1;
	mX = X;
	mY = Y;
	//for (S1.SnakePos;mX=0;)
	//{
	//	S1.mX = mX + 5;
	//	
	//}
	//return mX;
	//for (S1.SnakePos; mY = 0;)
	//{
	//	S1.mY = mY + 5;
	//}
	//return mY;
	return 1;
}

float Snake::NewPos(float X, float Y)
{
	Snake S1;
	mX = X; mY = Y;
	//S1.MoveSnake.SnakePos();
	return mX&&mY;

}
