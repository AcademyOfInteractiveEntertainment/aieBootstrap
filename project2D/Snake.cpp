#include "Snake.h"

Snake::Snake()
{
}

float Snake::SnakePos(float X, float Y)
{
	mX = X;
	mY = Y;
	return 0;


	float Snake::SnakePos(float X, float Y)
	{
		mX = X;
		mY = Y;
		return mX&&mY;
	}

	void Snake::changeDirection(char key)
	{
		{
			/*
			W
			A + D
			S

			1
			4 + 2
			3
			*/
			switch (key) {
			case 'w':
				break;
			case 'd':
				break;
			case 's':
				break;
			case 'a':
				break;
			}
		}

		float NewPos(float X, float Y);
	};

	}

	float Snake::NewPos(float X, float Y)
{
	Snake S1;
	mX = X; mY = Y;
	//S1.MoveSnake.SnakePos();
	return mX&&mY;

}
