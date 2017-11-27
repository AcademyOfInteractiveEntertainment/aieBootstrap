#pragma once
class PlayingArea
{
public:
	int mRows = 10;
	int mCols = 10;
	const int mapwidth = 20;
	const int mapheight = 20;
	int Grid(int Cols, int Rows);
	float mX; float mY;
	float snakePos(float x, float y);
	int DrawGrid();
	void generateFood(); 

};
