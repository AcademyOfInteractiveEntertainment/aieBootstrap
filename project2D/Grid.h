#pragma once
class PlayingArea
{
public:
	int mRows = 10;
	int mCols = 10;
	int Grid(int Cols, int Rows);
	float mX; float mY;
	float snakePos(float x, float y);
	int DrawGrid();

};
