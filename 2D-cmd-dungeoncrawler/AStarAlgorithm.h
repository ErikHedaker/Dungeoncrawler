#pragma once

#include "Vector2i.h"
#include <unordered_set>
#include <array>
#include <vector>

class Grid
{
	public:
		Grid( int maxCol, int maxRow, const std::vector<Vector2i>& obstacles );

		bool InBounds( const Vector2i& position ) const;
		bool Passable( const Vector2i& position ) const;
		std::vector<Vector2i> GetNeighbors( const Vector2i& position ) const;

	private:
		const int _maxCol;
		const int _maxRow;
		const std::unordered_set<Vector2i, Vector2iHasher> _obstacles;
};

struct Node
{
	Node( const Vector2i& position, int priority );

	Vector2i position;
	int priority;
};

struct CompareNodes
{
	bool operator()( const Node& lhs, const Node& rhs ) const;
};

int Heuristic( const Vector2i& from, const Vector2i& to );

std::vector<Vector2i> AStarAlgorithm( const Vector2i& start, const Vector2i& goal, int maxCol, int maxRow, const std::vector<Vector2i>& obstacles );