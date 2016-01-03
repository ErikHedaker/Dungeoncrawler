#include "AStarAlgorithmNew.h"

SquareGrid::SquareGrid( const Vector2i& gridSize, const std::vector<Vector2i>& obstaclePositions ) :
gridSize( gridSize )
{
	for( const auto& obstaclePosition : obstaclePositions )
	{
		obstacles.insert( obstaclePosition );
	}
}
bool SquareGrid::InBounds( const Vector2i& position ) const
{
	return
		0 <= position.col &&
		0 <= position.row &&
		position.col < gridSize.col &&
		position.row < gridSize.row;
}
bool SquareGrid::Passable( const Vector2i& position ) const
{
	return !obstacles.count( position );
}
const std::vector<Vector2i> SquareGrid::GetValidNeighbors( const Vector2i& position ) const
{
	std::vector<Vector2i> results;

	for( const auto& direction : DIRS )
	{
		Vector2i next = position + direction;

		if( InBounds( next ) &&
			Passable( next ) )
		{
			results.push_back( next );
		}
	}

	if( ( position.col + position.row ) % 2 == 0 )
	{
		// aesthetic improvement on square grids
		std::reverse( results.begin( ), results.end( ) );
	}

	return results;
}

const std::array<Vector2i, 4> SquareGrid::DIRS =
{
	Vector2i( 1, 0 ),	/* South */
	Vector2i( 0, -1 ),	/* West  */
	Vector2i( -1, 0 ),	/* North */
	Vector2i( 0, 1 )	/* East  */
};

int Heuristic( const Vector2i& a, const Vector2i& b )
{
	return abs( a.col - b.col ) + abs( a.row - b.row );
}

std::vector<Vector2i> AStarAlgorithmNew( const Vector2i& positionStart, const Vector2i& positionFinish, const Vector2i& gridSize, const std::vector<Vector2i>& obstaclePositions )
{
	/*
	http://www.redblobgames.com/pathfinding/a-star/implementation.html
	Algorithm copied from source and then rewritten.
	*/

	SquareGrid grid( gridSize, obstaclePositions );
	std::unordered_map<Vector2i, Vector2i> positionPrevious;
	std::unordered_map<Vector2i, int> positionCost;
	ModifiedPriorityQueue<Vector2i> frontier;
	frontier.Put( positionStart, 0 );

	positionPrevious[positionStart] = positionStart;
	positionCost[positionStart] = 0;

	while( !frontier.Empty( ) )
	{
		Vector2i positionCurrent = frontier.Get( );

		if( positionCurrent == positionFinish )
		{
			break;
		}

		for( const auto& next : grid.GetValidNeighbors( positionCurrent ) )
		{
			int costNew = positionCost[positionCurrent] + 1; // Position-in-grid cost goes here if needed.

			if( !positionCost.count( next ) || costNew < positionCost[next] )
			{
				int priority;

				positionCost[next] = costNew;
				priority = costNew + Heuristic( next, positionFinish );
				frontier.Put( next, priority );
				positionPrevious[next] = positionCurrent;
			}
		}
	}

	/* Reconstruct path */
	std::vector<Vector2i> path;
	Vector2i positionCurrent = positionFinish;

	path.push_back( positionCurrent );

	while( positionCurrent != positionStart )
	{
		positionCurrent = positionPrevious[positionCurrent];
		path.push_back( positionCurrent );
	}

	std::reverse( path.begin( ), path.end( ) );

	return path;
}