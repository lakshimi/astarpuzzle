//-------------------------------------------------------------------
//	File:		PQueue.h
//	Author:		Edan Choi
//	Date:		2014.11.24.
//	Contact:	http://sdev.tistory.com
//	Comment:	This class module is made for graph algorithm.
//	Update:
//
//-------------------------------------------------------------------

#pragma once
#ifndef _PQUEUE_H_
#define	_PQUEUE_H_
#include <memory.h>

///	@brief	Priority queue class.
///	@note	Class T must be derived from TPQueue::Node.
class PQueue
{
public:
	///	@brief	Priority queue node.
	class Node
	{
	friend PQueue;
	public:
		///	@brief	Constructor.
		Node() : index(-1) { }

		///	@brief	Compare priority value.
		virtual bool Compare(const Node *node) = 0;

	private:
		int index;								//<	heap index
	};

public:
	///	@brief	Constructor.
	PQueue() : maxCount(1024), curCount(0)
	{
		heap = new Node *[maxCount];
	}

	///	@brief	Destructor.
	~PQueue()
	{
		Empty();

		if( heap ) delete[] heap;
	}

	///	@brief	Add a value into heap.
	///	@param	node							[IN] node to be added
	void Add(Node *node)
	{
		int current;

		if( node->index == -1 )
		{
			current = curCount++;
			if( curCount > maxCount ) Increase();
		}
		else
		{
			current = node->index;
		}
	
		while( current > 0 )
		{
			int parent = (current-1)/2;
			if( !node->Compare(heap[parent]) ) break;
			heap[current] = heap[parent];
			heap[current]->index = current;
			current = parent;
		}

		heap[current] = node;
		node->index = current;
	}

	///	@brief	Get top node.
	Node *GetTop()
	{
		//	If heap is empty, return NULL.
		if( curCount == 0 ) return (Node *)0;

		Node *top = heap[0];
		Node *last = heap[--curCount];
		int s = 0, l = 1, r = 2, t = l;

		while( l < curCount )
		{
			if( r < curCount && heap[r]->Compare(heap[l]) ) t = r;
			if( last->Compare(heap[t]) ) break;
			heap[s] = heap[t];
			heap[s]->index = s;
			l = 2*t + 1, r = 2*t + 2, s = t, t = l;
		}
		heap[s] = last;
		last->index = s;

		return top;
	}

	///	@brief	Get element by index.
	Node *Get(int i)
	{
		if( i >= curCount ) return (Node *)0;
		return heap[i];
	}

	///	@brief	Check if this queue is empty.
	bool IsEmpty() const
	{
		return curCount == 0;
	}

	///	@brief	Empty queue.
	void Empty()
	{
		//for( int i = 0 ; i < curCount ; i++ )
		//{
		//	if( heap[i] ) delete heap[i];
		//}
		curCount = 0;
	}

	///	@brief	Increase queue size.
	void Increase()
	{
		maxCount <<= 1;
		Node **lastHeap = heap;
		heap = new Node *[maxCount];
		memcpy(heap, lastHeap, curCount*sizeof(Node *));
		delete[] lastHeap;
	}

private:
	Node **heap;								//<	heap data
	int maxCount;								//<	maximum count
	int curCount;								//<	current count
};

#endif
