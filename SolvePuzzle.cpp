//-------------------------------------------------------------------
//	File:		SolvePuzzle.cpp
//	Author:		Edan Choi
//	Date:		2014.12.12.
//	Contact:	http://sdev.tistory.com
//	Update:
//
//-------------------------------------------------------------------
#include "SolvePuzzle.h"
#include "PQueue.h"

#include <stdlib.h>

#define	HASHSIZE	31

inline char abs(char x) { return x<0?-x:x; }
int Move(char *squares, int rows, int cols, int emptySquare, eMove move);

class PuzzleNode : public PQueue::Node
{
public:
	PuzzleNode(char *csquares, int cEmptySquare, int cvalue) : value(cvalue), visited(false)
	{
		squares = csquares;
		emptySquare = cEmptySquare;
		est = 0;
		for( int i = 0 ; i < numSquares ; i++ ) 
		{
			if( squares[i] == 0 ) continue;
			int row = (squares[i]-1)/numCols;
			int col = (squares[i]-1)%numCols;
			est += abs(col - i%numCols) + abs(row - i/numCols);
		}
	}

	virtual ~PuzzleNode()
	{
		if( squares ) delete[] squares;
	}

	virtual bool Compare(const Node *node)
	{
		const PuzzleNode *cnode = (const PuzzleNode *)node;
		return( value+est <= cnode->value+cnode->est );
	}

	static void SetDimension(int rows, int cols)
	{
		numRows = rows;
		numCols = cols;
		numSquares = rows*cols;
	}

public:
	static int numRows, numCols, numSquares;
	char *squares;
	int emptySquare;
	int value, est;
	PuzzleNode *from;
	int cmd;
	bool visited;
	PuzzleNode *next;
};

int PuzzleNode::numRows;
int PuzzleNode::numCols;
int PuzzleNode::numSquares;

unsigned int GetHash(char *squares)
{
	unsigned int h = 0xc7423d43;
	for( int i = 0 ; i < PuzzleNode::numSquares ; i++ )
	{
		h |= squares[i];
		h = h<<11 | h>>21;
	}
	h %= HASHSIZE;
	return h;
}

void AddHash(PuzzleNode *hash[], PuzzleNode *node)
{
	unsigned int h = GetHash(node->squares);
	node->next = hash[h];
	hash[h] = node;
}

PuzzleNode *FindHash(PuzzleNode *hash[], char *squares)
{
	unsigned int h = GetHash(squares);
	for( PuzzleNode *tmp = hash[h] ; tmp ; tmp = tmp->next )
	{
		if( memcmp(tmp->squares, squares, PuzzleNode::numSquares*sizeof(char)) == 0 ) return tmp;
	}
	return (PuzzleNode *)0;
}

void SolvePuzzle(void *param)
{
	PuzzleParams *pp = (PuzzleParams *)param;

	PQueue queue;
	PuzzleNode *hash[HASHSIZE];

	PuzzleNode::SetDimension(pp->rows, pp->cols);

	memset(hash, 0, sizeof(hash));

	char *csquares = new char[pp->rows*pp->cols];
	memcpy(csquares, pp->squares, pp->rows*pp->cols*sizeof(char));

	PuzzleNode *node = new PuzzleNode(csquares, pp->emptySquare, 0);
	queue.Add(node);
	AddHash(hash, node);
	node->from = (PuzzleNode *)0;
	pp->numNodes = 1;

	while( queue.IsEmpty() == false )
	{
		node = (PuzzleNode *)queue.GetTop();
		node->visited = true;

		if( node->est == 0 ) break;

		for( int i = 0 ; i < 4 ; i++ )
		{
			char *newSquares = new char[PuzzleNode::numSquares];
			memcpy(newSquares, node->squares, PuzzleNode::numSquares*sizeof(char));
			int newEmptySquare = Move(newSquares, PuzzleNode::numRows, PuzzleNode::numCols, node->emptySquare, (eMove)i);
			if( node->emptySquare == newEmptySquare )
			{
				delete[] newSquares;
				continue;
			}
			PuzzleNode *t = FindHash(hash, newSquares);
			if( !t )
			{
				t = new PuzzleNode(newSquares, newEmptySquare, node->value+1);
				queue.Add(t);
				AddHash(hash, t);
				t->cmd = i;
				t->from = node;
				pp->numNodes++;
			}
			else if( t->visited == false && node->value + 1 < t->value )
			{
				delete[] newSquares;
				t->value = node->value+1;
				queue.Add(t);
				t->cmd = i;
				t->from = node;
			}
		}
	}

	int resultNum = 0;
	if( pp->result ) delete[] pp->result;
	pp->result = new char[node->value];
	for( PuzzleNode *t = node ; t->from ; t = t->from, resultNum++ )
	{
		pp->result[resultNum] = t->cmd;
	}
	pp->numResult = resultNum;

	for( int i = 0 ; i < HASHSIZE ; i++ )
	{
		while( hash[i] )
		{
			PuzzleNode *t = hash[i];
			hash[i] = t->next;
			delete t;
		}
	}
}

bool CanSolve(PuzzleParams *param)
{
	int n = param->rows*param->cols;
	int c = 0;

	for( int i = 0 ; i < n ; i++ )
	{
		if( param->squares[i] == 0 ) { c ^= i/param->cols; }
		else if( param->squares[i] < i+1 ) { c ^= 1; }
	}
	return (c & 1) == 0;
}

void GenerateRandomPuzzle(PuzzleParams *param)
{
	param->squares = new char[param->rows*param->cols];
	for( int i = 1 ; i < param->rows*param->cols ; i++ )
		param->squares[i-1] = i;
	param->emptySquare = param->rows*param->cols-1;
	param->squares[param->emptySquare] = 0;

	for( int i = 0 ; i < param->rows*param->cols*10 ; i++ )
		Move(param, (eMove)(rand()%4));
}

bool Move(PuzzleParams *param, eMove move)
{
	int newEmptySquare = Move(param->squares, param->rows, param->cols, param->emptySquare, move);
	if(param->emptySquare != newEmptySquare)
	{
		param->emptySquare = newEmptySquare;
		return true;
	}
	return false;
}

int Move(char *squares, int rows, int cols, int emptySquare, eMove move)
{
	int row = emptySquare/cols;
	int col = emptySquare%cols;
	int newEmptySquare = emptySquare;
	if( row < rows-1 && move == MoveUp ) newEmptySquare += cols;
	else if( col > 0 && move == MoveRight ) newEmptySquare -= 1;
	else if( row > 0 && move == MoveDown ) newEmptySquare -= cols;
	else if( col < cols-1 && move == MoveLeft ) newEmptySquare += 1;
	if( emptySquare != newEmptySquare )
	{
		squares[emptySquare] = squares[newEmptySquare];
		squares[emptySquare = newEmptySquare] = 0;
	}
	return emptySquare;
}