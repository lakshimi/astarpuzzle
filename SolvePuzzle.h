//-------------------------------------------------------------------
//	File:		SolvePuzzle.h
//	Author:		Edan Choi
//	Date:		2014.12.12.
//	Contact:	http://sdev.tistory.com
//	Update:
//
//-------------------------------------------------------------------
#pragma once

#ifndef _SOLVEPUZZLE_H_
#define	_SOLVEPUZZLE_H_

struct PuzzleParams
{
	int rows, cols;
	int numSquares;
	char *squares;
	int emptySquare;
	int numResult;
	char *result;
	int numNodes;
};

enum eMove
{
	MoveUp = 0,
	MoveRight,
	MoveDown,
	MoveLeft
};

void SolvePuzzle(void *param);
bool CanSolve(PuzzleParams *param);
bool Move(PuzzleParams *param, eMove move);
void GenerateRandomPuzzle(PuzzleParams *param);

#endif
