#pragma once

#include "Vector2i.h"
#include <vector>

template<class T>
class Array2D
{
	public:
	Array2D( const Vector2i& arraySize ) :
		_array( arraySize.col * arraySize.row ),
		_arraySize( arraySize )
	{ }
	Array2D( const Vector2i& arraySize, const T& data ) :
		_array( arraySize.col * arraySize.row, data ),
		_arraySize( arraySize )
	{ }

	const T GetAt( const Vector2i& position ) const
	{
		return _array[( position.row * _arraySize.col ) + position.col];
	}
	void SetAt( const Vector2i& position, const T& data )
	{
		_array[( position.row * _arraySize.col ) + position.col] = data;
	}

	private:
	std::vector<T> _array;
	const Vector2i _arraySize;
};

class Node
{
	public:
	Node( const Vector2i& position, int level, int priority );

	Vector2i position;
	int level;
	int priority;

	void UpdatePriority( const Vector2i& positionDest );
	void NextLevel( );
	int Estimate( const Vector2i& positionDestination ) const;
};

bool operator<( const Node& lhs, const Node& rhs );

std::vector<Vector2i> AStarAlgorithmOld( const Vector2i& positionStart, const Vector2i& positionFinish, const Vector2i& mapSize, const std::vector<Vector2i>& obstacles );