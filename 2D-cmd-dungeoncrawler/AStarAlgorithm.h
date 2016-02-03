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

class Grid
{
	public:
		Grid( int maxCol, int maxRow, const std::vector<Vector2i>& obstacles );

		bool InBounds( const Vector2i& position ) const;
		bool Passable( const Vector2i& position ) const;
		const std::vector<Vector2i> GetValidNeighbors( const Vector2i& position ) const;

	private:
		const int _maxCol;
		const int _maxRow;
		const std::array<Vector2i, 4> _directions;
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

int Heuristic( const Vector2i& positionFrom, const Vector2i& positionTo );

std::vector<Vector2i> AStarAlgorithm( const Vector2i& positionStart, const Vector2i& positionGoal, int maxCol, int maxRow, const std::vector<Vector2i>& obstacles );