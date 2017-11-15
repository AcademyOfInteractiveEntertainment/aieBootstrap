#include"Grid.h"
int PlayingArea::Grid(int Cols, int Rows)
{
	PlayingArea mGrid;
	mGrid.mRows = Rows;
	mGrid.mCols = Cols;
	return Rows&&Cols;
}

float PlayingArea::snakePos(float x, float y)
{
	mX = x; mY = y;
	return mX&&mY;
}

int PlayingArea::DrawGrid()
{
	PlayingArea mGrid;
	mGrid.mRows = mRows;
	mGrid.mCols = mCols;
	return mGrid.mRows&&mCols;
}
