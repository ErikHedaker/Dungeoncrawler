#include "AStarAlgorithm.h"
#include <iostream>

SquareGrid::SquareGrid( std::size_t maxCol, std::size_t maxRow, const std::vector<Vector2i>& obstacles ) :
	_maxCol( maxCol ),
	_maxRow( maxRow ),
	_obstacles( obstacles.begin( ), obstacles.end( ) ),
	_directions
	( { {
		Vector2i( 1, 0 ),
		Vector2i( 0, -1 ),
		Vector2i( -1, 0 ),
		Vector2i( 0, 1 )
	} } )
{ }
bool SquareGrid::InBounds( const Vector2i& position ) const
{
	return
		position.col >= 0 &&
		position.row >= 0 &&
		position.col < _maxCol &&
		position.row < _maxRow;
}
bool SquareGrid::Passable( const Vector2i& position ) const
{
	return !( _obstacles.count( position ) );
}
const std::vector<Vector2i> SquareGrid::GetValidNeighbors( const Vector2i& position ) const
{
	std::vector<Vector2i> results;

	for( const auto& direction : _directions )
	{
		Vector2i positionNeighbor = position + direction;

		if( InBounds( positionNeighbor ) &&
			Passable( positionNeighbor ) )
		{
			results.push_back( positionNeighbor );
		}
	}

	if( ( position.col + position.row ) % 2 == 0 )
	{
		// aesthetic improvement on square grids
		std::reverse( results.begin( ), results.end( ) );
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

int Heuristic( const Vector2i& positionFrom, const Vector2i& positionTo )
{
	return abs( positionFrom.col - positionTo.col ) + abs( positionFrom.row - positionTo.row );
}

std::vector<Vector2i> AStarAlgorithm( const Vector2i& positionStart, const Vector2i& positionGoal, std::size_t maxCol, std::size_t maxRow, const std::vector<Vector2i>& obstacles )
{
	/*
		http://www.redblobgames.com/pathfinding/a-star/implementation.html
		Algorithm copied from source and then rewritten.
	*/

	std::priority_queue<Node, std::vector<Node>, CompareNodes> activeNodes;
	std::unordered_map<Vector2i, Vector2i, Vector2iHasher> positionCameFrom;
	std::unordered_map<Vector2i, int, Vector2iHasher> positionCost;
	const SquareGrid grid( maxCol, maxRow, obstacles );

	activeNodes.emplace( positionStart, 0 );
	positionCameFrom[positionStart] = positionStart;
	positionCost[positionStart] = 0;

	while( !activeNodes.empty( ) )
	{
		const Node current = activeNodes.top( );

		activeNodes.pop( );

		if( current.position == positionGoal )
		{
			break;
		}

		for( const auto& positionNeighbor : grid.GetValidNeighbors( current.position ) )
		{
			const int newCost = positionCost[current.position] + 1; /* Position-in-grid cost goes here for weighted grid. */

			if( !positionCost.count( positionNeighbor ) /* Node hasn't been visited before */ ||
				newCost < positionCost[positionNeighbor] )
			{
				const int priority = newCost + Heuristic( positionNeighbor, positionGoal );

				activeNodes.emplace( positionNeighbor, priority );
				positionCameFrom[positionNeighbor] = current.position;
				positionCost[positionNeighbor] = newCost;
			}
		}
	}

	/* Reconstruct path */
	std::vector<Vector2i> path;
	Vector2i positionCurrent = positionGoal;

	path.push_back( positionCurrent );

	while( positionCurrent != positionStart )
	{
		try
		{
			positionCurrent = positionCameFrom.at( positionCurrent );
			path.push_back( positionCurrent );
		}
		catch( const std::exception& e )
		{
			std::cout << "\nException: " << e.what( );
			
			break;
		}
	}

	std::reverse( path.begin( ), path.end( ) );

	return path;
}