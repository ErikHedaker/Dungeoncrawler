#include "Dungeon.h"
#include "IO.h"
#include <random>
#include <queue>
#include <algorithm>
#include <Windows.h>

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

		const T& GetAt( const Vector2i& position ) const
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
		Node( const Vector2i& position, int level, int priority ) :
			position( position ),
			level( level ),
			priority( priority )
		{ }

		Vector2i position;
		int level;
		int priority;

		void UpdatePriority( const Vector2i& positionDest )
		{
			priority = level + Estimate( positionDest ) * 10000;
		}
		void NextLevel( )
		{
			level += 10;
		}
		int Estimate( const Vector2i& positionDestination ) const
		{
			return abs( position.col - positionDestination.col ) + abs( position.row - positionDestination.row );
		}
};
bool operator<( const Node& lhs, const Node& rhs )
{
	return lhs.priority < rhs.priority;
}

extern std::vector<Vector2i> AStarAlgorithm( const Vector2i& positionStart, const Vector2i& positionFinish, const Vector2i& mapSize, const std::vector<Vector2i>& obstacles )
{
	/*
	http://code.activestate.com/recipes/577457-a-star-shortest-path-algorithm/
	Algorithm copied from source and then rewritten
	*/

	Array2D<bool> mapClosedNodes( mapSize, false );
	Array2D<int> mapOpenNodes( mapSize, 0 );
	Array2D<int> mapDirections( mapSize, 0 );
	const int directionAmount = 4;
	const std::vector<Vector2i> directionValues
	{
		Vector2i( 1, 0 ),	/* East. */
		Vector2i( 0, 1 ),	/* North. */
		Vector2i( -1, 0 ),	/* West. */
		Vector2i( 0, -1 )	/* South. */
	};
	std::vector<Vector2i> path { positionStart };
	std::priority_queue<Node> pq[2];
	int pqi = 0; /* priority_queue index. */
	Node nodeParent( positionFinish, 0, 0 );

	nodeParent.UpdatePriority( positionStart );
	pq[0].push( nodeParent );
	mapOpenNodes.SetAt( nodeParent.position, nodeParent.priority );

	while( !pq[pqi].empty( ) )
	{
		nodeParent = pq[pqi].top( );
		pq[pqi].pop( );
		mapOpenNodes.SetAt( nodeParent.position, 0 );
		mapClosedNodes.SetAt( nodeParent.position, true );

		if( nodeParent.position == positionStart )
		{
			Vector2i position = nodeParent.position;

			while( position != positionFinish )
			{
				position = position + directionValues.at( mapDirections.GetAt( position ) );
				path.push_back( position );
			}

			return path;
		}

		/* Generate moves (child nodes) in all possible directions. */
		for( int i = 0; i < directionAmount; i++ )
		{
			Vector2i positionDirection( 0, 0 );
			positionDirection.col = nodeParent.position.col + directionValues[i].col;
			positionDirection.row = nodeParent.position.row + directionValues[i].row;

			if( positionDirection.col >= 0 &&
				positionDirection.col <= mapSize.col - 1 &&
				positionDirection.row >= 0 &&
				positionDirection.row <= mapSize.row - 1 &&
				mapClosedNodes.GetAt( positionDirection ) == false &&
				std::find( obstacles.begin( ), obstacles.end( ), positionDirection ) == obstacles.end( ) )
			{
				Node nodeChild( positionDirection, nodeParent.level, nodeParent.priority );
				nodeChild.NextLevel( );
				nodeChild.UpdatePriority( positionStart );

				if( mapOpenNodes.GetAt( positionDirection ) == 0 )
				{
					mapOpenNodes.SetAt( positionDirection, nodeChild.priority );
					pq[pqi].push( nodeChild );

					/* Mark its parent node direction. */
					mapDirections.SetAt( positionDirection, ( i + directionAmount / 2 ) % directionAmount );
				}
				else if( mapOpenNodes.GetAt( positionDirection ) > nodeChild.priority )
				{
					/* Update the priority and parent direction info. */
					mapOpenNodes.SetAt( positionDirection, nodeChild.priority );
					mapDirections.SetAt( positionDirection, ( i + directionAmount / 2 ) % directionAmount );

					/*
					* Replace the node by emptying one pq to the other one
					* except the node to be replaced will be ignored
					* and the new node will be pushed in instead.
					*/
					while( pq[pqi].top( ).position != positionDirection )
					{
						pq[1 - pqi].push( pq[pqi].top( ) );
						pq[pqi].pop( );
					}

					/* Remove the wanted node and empty the larger size pq to the smaller one. */
					pq[pqi].pop( );
					if( pq[pqi].size( ) > pq[1 - pqi].size( ) )
					{
						pqi = 1 - pqi;
					}

					while( !pq[pqi].empty( ) )
					{
						pq[1 - pqi].push( pq[pqi].top( ) );
						pq[pqi].pop( );
					}

					pqi = 1 - pqi;

					/* Add the better node instead. */
					pq[pqi].push( nodeChild );
				}
			}
		}
	}

	/* No route found. */
	return path;
}
extern int RandomNumberGenerator( int min, int max )
{
	static std::random_device randomEngine;
	std::mt19937 generator( randomEngine( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}
extern int RandomPositiveNegativeGenerator( )
{
	return RandomNumberGenerator( 0, 1 ) ? 1 : -1;
}

void SetWindow( int Width, int Height )
{
	_COORD coord;
	coord.X = Width;
	coord.Y = Height;

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = Height - 1;
	Rect.Right = Width - 1;

	HANDLE Handle = GetStdHandle( STD_OUTPUT_HANDLE );      // Get Handle 
	SetConsoleScreenBufferSize( Handle, coord );            // Set Buffer Size 
	SetConsoleWindowInfo( Handle, TRUE, &Rect );            // Set Window Size 
}

extern const Vector2i WINDOWSIZE( 100, 50 );

int main( )
{
	SetWindow( WINDOWSIZE.col, WINDOWSIZE.row );

	while( true )
	{
		Dungeon dungeon;
		char gameType;

		IO::OutputClearScreen( );
		IO::OutputStartMessage( );
		IO::OutputGameTypes( );

		gameType = IO::InputValidChar( "\nYour choice: ", std::vector<char>{ '1', '2', '3' } );
		if( gameType == '3' )
		{
			break;
		}

		dungeon.Build( gameType );
		dungeon.GameLoop( );
	}

	return 0;
}