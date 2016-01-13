#include "AStarAlgorithm.h"
#include "IO.h"

void Vector2iHasher::HashCombine( std::size_t& seed, int value )
{
	/*
		https://www.quora.com/How-can-I-declare-an-unordered-set-of-pair-of-int-int-in-C++11
		Function copied from source and then rewritten.
	*/

	std::hash<int> hasher;

	seed ^= hasher( value ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
}
std::size_t Vector2iHasher::operator()( const Vector2i& key )
{
	size_t seed = 0;

	HashCombine( seed, key.col );
	HashCombine( seed, key.row );

	return seed;
}

SquareGrid::SquareGrid( const Vector2i& gridSize, const std::vector<Vector2i>& obstaclePositions ) :
	gridSize( gridSize ),
	obstacles( obstaclePositions.begin( ), obstaclePositions.end( ) ),
	directions
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

	for( const auto& direction : directions )
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

std::vector<Vector2i> AStarAlgorithm( const Vector2i& positionStart, const Vector2i& positionGoal, const Vector2i& gridSize, const std::vector<Vector2i>& obstaclePositions )
{
	/*
		http://www.redblobgames.com/pathfinding/a-star/implementation.html
		Algorithm copied from source and then rewritten.
	*/

	std::priority_queue<Node, std::vector<Node>, CompareNodes> activeNodes;
	std::unordered_map<Vector2i, Vector2i, Vector2iHasher> positionCameFrom;
	std::unordered_map<Vector2i, int, Vector2iHasher> positionCost;
	const SquareGrid grid( gridSize, obstaclePositions );

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