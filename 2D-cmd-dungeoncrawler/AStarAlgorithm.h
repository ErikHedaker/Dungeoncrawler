#pragma once

#include "Vector2i.h"
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <vector>
#include <utility>
#include <queue>
#include <algorithm>
#include <functional>

/*
	https://www.quora.com/How-can-I-declare-an-unordered-set-of-pair-of-int-int-in-C++11
	Functions copied from source and then rewritten.
*/
template <class T>
void hash_combine( std::size_t & seed, const T & v )
{
	std::hash<T> hasher;
	seed ^= hasher( v ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
}
namespace std
{
	template<>
	struct hash<Vector2i>
	{
		typedef Vector2i argument_type;
		typedef std::size_t result_type;
		result_type operator()( argument_type const& s ) const
		{
			size_t seed = 0;
			::hash_combine( seed, s.col );
			::hash_combine( seed, s.row );
			return seed;
		}
	};
}

struct SquareGrid
{
	SquareGrid( const Vector2i& gridSize, const std::vector<Vector2i>& obstaclePositions );

	Vector2i gridSize;
	std::unordered_set<Vector2i> obstacles;
	static const std::array<Vector2i, 4> DIRS;

	bool InBounds( const Vector2i& position ) const;
	bool Passable( const Vector2i& position ) const;
	const std::vector<Vector2i> GetValidNeighbors( const Vector2i& position ) const;
};

struct Node
{
	Node( const Vector2i& position, int priority );

	Vector2i position;
	int priority;
};

struct CompareNodes
{
	bool operator()( const Node& lhs, const Node& rhs );
};
bool operator>( const Node& lhs, const Node& rhs );

int Heuristic( const Vector2i& a, const Vector2i& b );

std::vector<Vector2i> AStarAlgorithm( const Vector2i& positionStart, const Vector2i& positionFinish, const Vector2i& gridSize, const std::vector<Vector2i>& obstaclePositions );