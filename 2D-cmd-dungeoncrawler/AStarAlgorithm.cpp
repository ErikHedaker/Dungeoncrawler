#include "AStarAlgorithm.h"
#include "IO.h"

SquareGrid::SquareGrid( const Vector2i& gridSize, const std::vector<Vector2i>& obstaclePositions ) :
	gridSize( gridSize )
{
	for( const auto& position : obstaclePositions )
	{
		obstacles.insert( position );
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

Node::Node( const Vector2i& position, int priority ) :
	position( position ),
	priority( priority )
{ }

bool CompareNodes::operator()( const Node& lhs, const Node& rhs )
{
	return lhs.priority > rhs.priority;
}

int Heuristic( const Vector2i& a, const Vector2i& b )
{
	return abs( a.col - b.col ) + abs( a.row - b.row );
}

std::vector<Vector2i> AStarAlgorithm( const Vector2i& positionStart, const Vector2i& positionFinish, const Vector2i& gridSize, const std::vector<Vector2i>& obstaclePositions )
{
	/*
		http://www.redblobgames.com/pathfinding/a-star/implementation.html
		Algorithm copied from source and then rewritten.
	*/

	SquareGrid grid( gridSize, obstaclePositions );
	std::priority_queue<Node, std::vector<Node>, CompareNodes> pQueue;
	std::unordered_map<Vector2i, Vector2i> positionCameFrom;
	std::unordered_map<Vector2i, int> positionCost;

	pQueue.emplace( positionStart, 0 );
	positionCameFrom[positionStart] = positionStart;
	positionCost[positionStart] = 0;

	while( !pQueue.empty( ) )
	{
		Node current = pQueue.top( );

		pQueue.pop( );

		if( current.position == positionFinish )
		{
			break;
		}

		for( const auto& next : grid.GetValidNeighbors( current.position ) )
		{
			int newCost = positionCost[current.position] + 1; // Position-in-grid cost goes here if needed.

			if( !positionCost.count( next ) ||
				newCost < positionCost[next] )
			{
				int priority = newCost + Heuristic( next, positionFinish );

				pQueue.emplace( next, priority );
				positionCameFrom[next] = current.position;
				positionCost[next] = newCost;
			}
		}
	}

	/* Reconstruct path */
	std::vector<Vector2i> path;
	Vector2i positionCurrent = positionFinish;

	path.push_back( positionCurrent );

	while( positionCurrent != positionStart )
	{
		try
		{
			positionCurrent = positionCameFrom.at( positionCurrent );
			path.push_back( positionCurrent );
		}
		catch( const std::out_of_range& e )
		{
			Output::String( "\n\n" );
			Output::String( e.what( ) );
			Output::String( " (incomplete path).\n" );
			break;
		}
	}

	std::reverse( path.begin( ), path.end( ) );

	return path;
}