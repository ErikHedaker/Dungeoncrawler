#include "Room.h"
#include <iostream>
#include <string>
#include <math.h>

extern int GetPositiveInteger( const std::string& context );
extern int RandomNumberGenerator( int min, int max );
extern int RandomPositiveNegativeGenerator( );

bool Room::EmptySurroundedTile( const Vector2i& current ) const
{
	Vector2i iterator;
	int surroundingWalls = 0;

	for( iterator.row = current.row - 1; iterator.row <= current.row + 1; iterator.row++ )
	{
		for( iterator.col = current.col - 1; iterator.col <= current.col + 1; iterator.col++ )
		{
			if( GetVisibleDataAt( iterator ) == Portrait::Wall )
			{
				surroundingWalls++;
			}
		}
	}

	return surroundingWalls > 4;
}

void Room::BuildHiddenData( )
{
	_hiddenData.clear( );
	_hiddenData.resize( _maxCols * _maxRows, '0' );
}
void Room::UpdateHiddenDataAt( const Vector2i& position, char icon )
{
	_hiddenData[( position.row * _maxCols ) + position.col] = icon;
}
char Room::GetHiddenDataAt( const Vector2i& position ) const
{
	return _hiddenData[( position.row * _maxCols ) + position.col];
}

void Room::BuildVisibleData( )
{
	_visibleData.clear( );
	_visibleData.resize( _maxCols * _maxRows, '-' );
}
void Room::UpdateVisibleDataAt( const Vector2i& position, char icon )
{
	_visibleData[( position.row * _maxCols ) + position.col] = icon;
}
char Room::GetVisibleDataAt( const Vector2i& position ) const
{
	return _visibleData[( position.row * _maxCols ) + position.col];
}

void Room::BuildFogOfWarData( )
{
	_fogOfWarData.clear( );
	_fogOfWarData.resize( _maxCols * _maxRows, false );
}
void Room::UpdateFogOfWarData( )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _maxRows; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _maxCols; iterator.col++ )
		{
			if( iterator >= player.GetPosition( ) - _lineOfSight &&
				iterator <= player.GetPosition( ) + _lineOfSight )
			{
				_fogOfWarData[( iterator.row * _maxCols ) + iterator.col] = true;
			}
		}
	}
}
bool Room::GetFogOfWarDataAt( const Vector2i& position ) const
{
	return _fogOfWarData[( position.row * _maxCols ) + position.col];
}

void Room::SetRoomSize( int gameType )
{
	if( gameType == 2 )
	{
		RETRY:
		_maxCols = GetPositiveInteger( "Enter the Room width: " );
		_maxRows = GetPositiveInteger( "Enter the Room heigth: " );

		if( _maxCols < 3 ||
			_maxRows < 3 )
		{
			std::cout << "\nInput is too small, try again.\n\n";
			goto RETRY;
		}
	}
	else
	{
		_maxCols = RandomNumberGenerator( 50, 100 );
		_maxRows = RandomNumberGenerator( 15, 30 );
	}
}
void Room::SetRoomLineOfSight( int gameType )
{
	if( gameType == 2 )
	{
		_lineOfSight = GetPositiveInteger( "Enter the line of sight range: " );
	}
	else
	{
		_lineOfSight = 4 ;
	}
}
void Room::SetRoomMonsterAmount( int gameType )
{
	if( gameType == 2 )
	{
		int amountMonsters;
		int maxMonsters = sqrt( _maxRows * _maxCols ) * 11 - 100;

		while( true )
		{
			amountMonsters = GetPositiveInteger( "Enter the amount of monsters: " );

			if( amountMonsters <= maxMonsters )
			{
				monsters.resize( amountMonsters );
				break;
			}
			else
			{
				std::cout << "\nToo many monsters, try again.\n\n";
			}
		}
	}
	else
	{
		double high = sqrt( _maxRows * _maxCols ) / 1.5;
		double low = sqrt( _maxRows * _maxCols ) / 3;
		monsters.resize( RandomNumberGenerator( static_cast<int>( low ), static_cast<int>( high ) ) );
	}

	std::cout << "\n";
}

void Room::SetPlayerPosition( )
{
	Vector2i newPosition( 1, 1 );
	player.SetPosition( newPosition );
	UpdateVisibleDataAt( newPosition, Portrait::Player );
}
void Room::SetExits( )
{
	Vector2i position( _maxCols - 2, _maxRows - 1 );
	exits.emplace_back( position );
	UpdateVisibleDataAt( position, Portrait::Exit );
}
void Room::SetOuterWalls( )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _maxRows; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _maxCols; iterator.col++ )
		{
			if( GetVisibleDataAt( iterator ) != '=' &&
				( iterator.row == _maxRows - 1 ||
				  iterator.col == _maxCols - 1 ||
				  iterator.row == 0 ||
				  iterator.col == 0 ) )
			{
				walls.emplace_back( iterator );
				UpdateVisibleDataAt( iterator, Portrait::Wall );
			}
		}
	}
}
void Room::SetInvisiblePath( )
{
	/*
	 Path from player starting point to exit.
	 Walls cannot be placed on path.
	 player can no longer be cut off from exit due to randomly placed walls.
	 Function can be replaced by A* algorithm, or similar algorithm.
	*/

	Vector2i newPosition = player.GetPosition( );
	Vector2i oldPosition;
	double high = sqrt( _maxCols * _maxRows ) * 3;
	double low = sqrt( _maxCols * _maxRows ) * 2;
	int threshold = RandomNumberGenerator( static_cast<int>( low ), static_cast<int>( high ) );
	int thresholdCounter = 0;
	int random;

	if( paths.size( ) == 0 )
	{
		paths.emplace_back( player.GetPosition( ) );
		UpdateHiddenDataAt( player.GetPosition( ), Portrait::Path );
	}

	while( true )
	{
		oldPosition = newPosition;
		random = RandomNumberGenerator( 1, 16 );

		if( thresholdCounter < threshold ) // Equal chance to move in any direction.
		{
			if( random <= 4 )
			{
				newPosition.row++;
			}
			else if( random <= 8 )
			{
				newPosition.col++;
			}
			else if( random <= 12 )
			{
				newPosition.row--;
			}
			else
			{
				newPosition.col--;
			}

			thresholdCounter++;
		}
		else // Increased chance to move right and down.
		{
			if( random <= 6 )
			{
				newPosition.row++;
			}
			else if( random <= 14 )
			{
				newPosition.col++;
			}
			else if( random <= 15 )
			{
				newPosition.row--;
			}
			else
			{
				newPosition.col--;
			}
		}

		if( GetVisibleDataAt( newPosition ) == Portrait::Wall )
		{
			newPosition = oldPosition;
		}
		else
		{
			paths.emplace_back( newPosition );
			UpdateHiddenDataAt( newPosition, Portrait::Path );
		}

		if( paths.back( ).GetPosition( ) == exits.back( ).GetPosition( ) )
		{
			break;
		}
	}
}
void Room::SetRandomWalls( )
{
	Vector2i iterator;
	Vector2i position;
	int infiniteLoopBreaker = 0;
	int printLoadingBar = 0;
	int sourceWallsLeft = ( _maxCols * _maxRows ) / 20;
	int extensionWallsLeft = ( _maxCols * _maxRows ) / 4;

	std::cout << "\n\nLoading source walls.\n";

	while( sourceWallsLeft > 0 )
	{
		position.col = RandomNumberGenerator( 1, _maxCols - 2 );
		position.row = RandomNumberGenerator( 1, _maxRows - 2 );

		if( GetVisibleDataAt( position ) == Portrait::Floor &&
			GetHiddenDataAt( position ) != Portrait::Path )
		{
			walls.emplace_back( position );
			UpdateVisibleDataAt( position, Portrait::Wall );
			sourceWallsLeft--;

			if( ++printLoadingBar == 5000 )
			{
				printLoadingBar = 0;
				std::cout << ".";
			}
		}

		if( ++infiniteLoopBreaker > _maxCols * _maxRows )
		{
			break;
		}
	}

	infiniteLoopBreaker = 0;

	std::cout << "\n\nLoading extension walls.\n";

	while( extensionWallsLeft > 0 )
	{
		for( int i = 0; i < walls.size( ); i++ )
		{
			position = walls[i].GetPosition( );

			if( position.col == _maxCols - 1 ||
				position.row == _maxRows - 1 ||
				position.col == 0 ||
				position.row == 0 )
			{
				continue;
			}

			position.col += RandomNumberGenerator( 0, 1 ) * RandomPositiveNegativeGenerator( );
			position.row += RandomNumberGenerator( 0, 1 ) * RandomPositiveNegativeGenerator( );

			if( GetVisibleDataAt( position ) == Portrait::Floor &&
				GetHiddenDataAt( position ) != Portrait::Path )
			{
				walls.emplace_back( position );
				UpdateVisibleDataAt( position, Portrait::Wall );
				extensionWallsLeft--;

				if( ++printLoadingBar == 5000 )
				{
					printLoadingBar = 0;
					std::cout << ".";
				}
			}
		}

		if( ++infiniteLoopBreaker > _maxCols * _maxRows )
		{
			break;
		}
	}

	std::cout << "\n\nLoading filler walls.\n";

	for( iterator.row = 1; iterator.row < _maxRows - 1; iterator.row++ )
	{
		for( iterator.col = 1; iterator.col < _maxCols - 1; iterator.col++ )
		{
			if( GetVisibleDataAt( iterator ) == Portrait::Floor &&
				GetHiddenDataAt( iterator ) != Portrait::Path &&
				EmptySurroundedTile( iterator ) == true )
			{
				walls.emplace_back( iterator );
				UpdateVisibleDataAt( iterator, Portrait::Wall );

				if( ++printLoadingBar == 5000 )
				{
					printLoadingBar = 0;
					std::cout << ".";
				}
			}
		}
	}
}
void Room::SetRandomMonsterPositions( )
{
	Vector2i position;
	int infiniteLoopBreaker = 0;
	int printLoadingBar = 0;

	std::cout << "\n\nLoading monsters.\n";

	for( unsigned int i = 0; i < monsters.size( ); i++ )
	{
		while( true )
		{
			position.col = RandomNumberGenerator( 1, _maxCols - 2 );
			position.row = RandomNumberGenerator( 1, _maxRows - 2 );

			if( GetVisibleDataAt( position ) == Portrait::Floor )
			{
				monsters[i].SetPosition( position );
				UpdateVisibleDataAt( position, Portrait::Monster );

				if( ++printLoadingBar == 500 )
				{
					printLoadingBar = 0;
					std::cout << ".";
				}

				break;
			}

			if( infiniteLoopBreaker++ > 100000 )
			{
				break;
			}
		}
	}
}

void Room::DrawRoom_Iterator1D( ) const
{
	unsigned int iterator;

	system( "CLS" );

	for( iterator = 0; iterator < _maxRows * _maxCols; ++iterator )
	{
		if( _fogOfWarData[iterator] == true )
		{
			std::cout << _visibleData[iterator];
		}
		else
		{
			std::cout << " ";
		}

		if( ( iterator + 1 ) % _maxCols == 0 )
		{
			std::cout << "\n";
		}
	}

	std::cout << "\n";
}
void Room::DrawRoom_Iterator2D( ) const
{
	Vector2i iterator;

	system( "CLS" );

	for( iterator.row = 0; iterator.row < _maxRows; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _maxCols; iterator.col++ )
		{
			if( GetFogOfWarDataAt( iterator ) == true )
			{
				std::cout << GetVisibleDataAt( iterator );
			}
			else
			{
				std::cout << " ";
			}
		}

		std::cout << "\n";
	}

	std::cout << "\n";
}
char Room::TurnOptions( )
{
	Vector2i newPosition;
	std::string userChoice;

	std::cout << "[W] Go up\n";
	std::cout << "[S] Go down\n";
	std::cout << "[A] Go left\n";
	std::cout << "[D] Go Right\n";
	std::cout << "[Q] Do nothing\n";
	std::cout << "[E] Exit to meny\n";

	REtry:
	std::cout << "\nYour choice: ";
	std::cin >> userChoice;

	switch( userChoice[0] )
	{
		case 'W':
		case 'w':       // Move up.
		{
			newPosition.row = player.GetPosition( ).row - 1;
			newPosition.col = player.GetPosition( ).col;

			break;
		}

		case 'S':
		case 's':       // Move down.
		{
			newPosition.row = player.GetPosition( ).row + 1;
			newPosition.col = player.GetPosition( ).col;

			break;
		}

		case 'A':
		case 'a':       // Move left.
		{
			newPosition.row = player.GetPosition( ).row;
			newPosition.col = player.GetPosition( ).col - 1;

			break;
		}

		case 'D':
		case 'd':       // Move right.
		{
			newPosition.row = player.GetPosition( ).row;
			newPosition.col = player.GetPosition( ).col + 1;

			break;
		}

		case 'Q':
		case 'q':       // Do nothing.
		{
			return 0;
		}

		case 'E':
		case 'e':       // Exit game.
		{
			system( "CLS" );

			return 'E';
		}

		default:
		{
			std::cout << "Invalid input, try again.\n";
			goto REtry;
		}
	}

	if( GetVisibleDataAt( newPosition ) != Portrait::Wall )
	{
		UpdateVisibleDataAt( player.GetPosition( ), Portrait::Floor );
		player.SetPosition( newPosition );
		UpdateVisibleDataAt( newPosition, Portrait::Player );
	}

	return 0;
}
void Room::RandomizeMonsterMovement( )
{
	Vector2i newPosition;
	int random;

	for( unsigned int i = 0; i < monsters.size( ); i++ )
	{
		while( true )
		{
			random = RandomNumberGenerator( 1, 8 );		// 25% to move, 75% to stand still.
			newPosition = monsters[i].GetPosition( );

			if( random == 1 )							// Monster moves vertically.
			{
				newPosition.row = newPosition.row + RandomPositiveNegativeGenerator( );
			}
			else if( random == 2 )						// Monster moves horizontally.
			{
				newPosition.col = newPosition.col + RandomPositiveNegativeGenerator( );
			}
			else										// Monster stands still.
			{
				break;
			}

			if( GetVisibleDataAt( newPosition ) != Portrait::Wall &&
				GetVisibleDataAt( newPosition ) != Portrait::Exit &&
				GetVisibleDataAt( newPosition ) != Portrait::Monster )
			{
				if( !( monsters[i].GetPosition( ) == player.GetPosition( ) ) )						// player movement is executed before monster movement, so
				{																					// clearing the monster's previous position after the player
					UpdateVisibleDataAt( monsters[i].GetPosition( ), Portrait::Floor );				// has moved to that position will make the player icon invisible.
				}																					// (this update avoids that)

				monsters[i].SetPosition( newPosition );
				UpdateVisibleDataAt( newPosition, Portrait::Monster );
				break;
			}
		}
	}
}

bool Room::CheckWinCondition( ) const
{
	for( unsigned int i = 0; i < exits.size( ); i++ )
	{
		if( player.GetPosition( ) == exits[i].GetPosition( ) )
		{
			std::cout << "You win!";
			std::cout << "\n\nPress enter to continue: ";
			std::cin.get( );
			std::cin.get( );

			return true;
		}
	}

	return false;
}
bool Room::CheckLoseCondition( ) const
{
	for( unsigned int i = 0; i < monsters.size( ); i++ )
	{
		if( player.GetPosition( ) == monsters[i].GetPosition( ) )
		{
			std::cout << "You lose!";
			std::cout << "\n\nPress enter to continue: ";
			std::cin.get( );
			std::cin.get( );

			return true;
		}
	}

	return false;
}