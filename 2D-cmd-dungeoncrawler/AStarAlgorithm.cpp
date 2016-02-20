#include "AStarAlgorithm.h"
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <vector>

Grid::Grid( int maxCol, int maxRow, const std::vector<Vector2i>& obstacles ) :
	_maxCol( maxCol ),
	_maxRow( maxRow ),
	_obstacles( obstacles.begin( ), obstacles.end( ) )
{ }
bool Grid::InBounds( const Vector2i& position ) const
{
	return
		position.col >= 0 &&
		position.row >= 0 &&
		position.col < _maxCol &&
		position.row < _maxRow;
}
bool Grid::Passable( const Vector2i& position ) const
{
	return !( _obstacles.count( position ) );
}
std::vector<Vector2i> Grid::GetNeighbors( const Vector2i& position ) const
{
	const std::array<Vector2i, 4> directions =
	{ {
		{  0, -1 },
		{ -1,  0 },
		{  0,  1 },
		{  1,  0 }
	} };
	std::vector<Vector2i> results;

	for( const auto& direction : directions )
	{
		Vector2i neighbor = position + direction;

		if( InBounds( neighbor ) &&
			Passable( neighbor ) )
		{
			results.push_back( neighbor );
		}

		if( ( position.col + position.row ) % 2 == 0 )
		{
			/* Aesthetic improvement on square grids according to original author */
			std::reverse( results.begin( ), results.end( ) );
		}
	}

	return results;
}

Node::Node( const Vector2i& position, int priority ) :
	position( position ),
	priority( priority )
{ }

bool CompareNodes::operator()( const Node& lhs, const Node& rhs ) const
{
	return lhs.priority > rhs.priority;
}

int Heuristic( const Vector2i& from, const Vector2i& to )
{
	return abs( from.col - to.col ) + abs( from.row - to.row );
}

std::vector<Vector2i> AStarAlgorithm( const Vector2i& start, const Vector2i& goal, int maxCol, int maxRow, const std::vector<Vector2i>& obstacles )
{
	/*
		http://www.redblobgames.com/pathfinding/a-star/implementation.html
		My implementation of the A* algorithm is based on this article
	*/

	std::priority_queue<Node, std::vector<Node>, CompareNodes> activeNodes;
	std::unordered_map<Vector2i, Vector2i, Vector2iHasher> positionCameFrom;
	std::unordered_map<Vector2i, int, Vector2iHasher> positionCost;
	const Grid grid( maxCol, maxRow, obstacles );

	activeNodes.emplace( start, 0 );
	positionCameFrom[start] = start;
	positionCost[start] = 0;

	while( !activeNodes.empty( ) )
	{
		const Node current = activeNodes.top( );

		activeNodes.pop( );

		if( current.position == goal )
		{
			break;
		}

		for( const auto& neighbor : grid.GetNeighbors( current.position ) )
		{
			const int newCost = positionCost[current.position] + 1; /* Position-in-grid cost goes here for weighted grid. */

			if( !positionCost.count( neighbor ) ||
				newCost < positionCost[neighbor] )
			{
				const int priority = newCost + Heuristic( neighbor, goal );

				activeNodes.emplace( neighbor, priority );
				positionCameFrom[neighbor] = current.position;
				positionCost[neighbor] = newCost;
			}
		}
	}

	/* Reconstruct path */
	const Vector2i notSet = { -1, -1 };
	Vector2i current = goal;
	std::vector<Vector2i> path;

	path.push_back( current );

	while( current != start &&
		   positionCameFrom.at( current ) != notSet )
	{
		current = positionCameFrom.at( current );
		path.push_back( current );
	}

	std::reverse( path.begin( ), path.end( ) );

	return path;
}