#include "Room.h"
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

extern const char iconPlayer;
extern const char iconMonster;
extern const char iconWall;
extern const char iconExit;
extern const char iconPath;

extern int GetPositiveInteger( );
extern int RandomNumberGenerator( int min, int max );
extern int RandomPositiveNegativeGenerator( );

bool Room::CheckPosition( Entity& current, Entity& search )
{
	if( current.xPosition == search.xPosition &&
		current.yPosition == search.yPosition )
	{
		return true;
	}

	return false;
}
bool Room::CheckPosition( int xCurrent, int yCurrent, Entity& search )
{
	if( xCurrent == search.xPosition &&
		yCurrent == search.yPosition )
	{
		return true;
	}

	return false;
}
bool Room::CheckEmptySurroundedTile( int xCurrent, int yCurrent )
{
	int surroundingWalls = 0;

	for( int y = yCurrent - 1; y <= yCurrent + 1; y++ )
	{
		for( int x = xCurrent - 1; x <= xCurrent + 1; x++ )
		{
			if( GetVisibleDataMap( x, y ) == iconWall )
			{
				surroundingWalls++;
			}
		}
	}

	return surroundingWalls > 4;
}

void Room::SetLength( int newLength )
{
	_length = newLength;
}
int Room::GetLength( )
{
	return _length;
}

void Room::SetWidth( int newWidth )
{
	_width = newWidth;
}
int Room::GetWidth( )
{
	return _width;
}

void Room::SetLineOfSight( int newLOS )
{
	_lineOfSight = newLOS;
}
int Room::GetLineOfSight( )
{
	return _lineOfSight;
}

void Room::BuildHiddenDataMap( )
{
	_hiddenDataMap.clear( );
	_hiddenDataMap.resize( _length, std::vector<char>( _width, '0' ) );
}
void Room::SetHiddenDataMap( int x, int y, char value )
{
	_hiddenDataMap[y][x] = value;
}
char Room::GetHiddenDataMap( int x, int y )
{
	return _hiddenDataMap[y][x];
}

void Room::BuildVisibleDataMap( )
{
	_visibleDataMap.clear( );
	_visibleDataMap.resize( _length, std::vector<char>( _width, '-' ) );
}
void Room::SetVisibleDataMap( int x, int y, char value )
{
	_visibleDataMap[y][x] = value;
}
char Room::GetVisibleDataMap( int x, int y )
{
	return _visibleDataMap[y][x];
}

void Room::BuildFogOfWarDataMap( )
{
	_fogOfWarDataMap.clear( );
	_fogOfWarDataMap.resize( _length, std::vector<char>( _width, 0 ) );
}
void Room::SetFogOfWarDataMap( )
{
	for( int y = 0; y < _length; y++ )
	{
		for( int x = 0; x < _width; x++ )
		{
			if( x >= player.xPosition - _lineOfSight && x <= player.xPosition + _lineOfSight &&
				y >= player.yPosition - _lineOfSight && y <= player.yPosition + _lineOfSight )
			{
				_fogOfWarDataMap[y][x] = 1;
			}
		}
	}
}
char Room::GetFogOfWarDataMap( int x, int y )
{
	return _fogOfWarDataMap[y][x];
}

void Room::SetRoomSize( int roomMode )
{
	if( roomMode == 2 )
	{
		RETRY:
		std::cout << "Enter the playing Room length: ";
		SetLength( GetPositiveInteger( ) );
		std::cout << "Enter the playing Room width: ";
		SetWidth( GetPositiveInteger( ) );

		if( GetLength( ) < 3 ||
			GetWidth( ) < 3 )
		{
			std::cout << "\nInput is too small, try again.\n\n";
			goto RETRY;
		}
	}
	else
	{
		SetLength( RandomNumberGenerator( 15, 30 ) );
		SetWidth( RandomNumberGenerator( 50, 100 ) );

	}
}
void Room::SetRoomLineOfSight( int roomMode )
{
	if( roomMode == 2 )
	{
		std::cout << "Enter the line of sight range: ";
		SetLineOfSight( GetPositiveInteger( ) );
	}
	else
	{
		SetLineOfSight( 4 );
	}
}
void Room::SetMonsterAmount( int roomMode )
{
	if( roomMode == 2 )
	{
		int amountMonsters;
		int minimumMonsters = ( GetLength( ) * GetWidth( ) ) - wall.size( ) - ( GetLength( ) * GetWidth( ) ) / 2;

		while( true )
		{
			std::cout << "Enter the amount of monsters: ";
			amountMonsters = GetPositiveInteger( );

			if( amountMonsters >= minimumMonsters &&
				amountMonsters != 0 )
			{
				std::cout << "Too many monsters, try again.\n";
			}
			else
			{
				monster.resize( amountMonsters );
				std::cout << "\n";
				break;
			}
		}
	}
	else
	{
		double high = sqrt( GetLength( ) * GetWidth( ) ) / 1.5;
		double low = sqrt( GetLength( ) * GetWidth( ) ) / 3;
		monster.resize( RandomNumberGenerator( static_cast<int>( low ), static_cast<int>( high ) ) );
	}
}

void Room::SetPlayerPosition( )
{
	player.xPosition = 1;
	player.yPosition = 1;
	SetVisibleDataMap( 1, 1, iconPlayer );
}
void Room::SetExits( )
{
	int xTemp = GetWidth( ) - 2;
	int yTemp = GetLength( ) - 1;
	exit.emplace_back( xTemp, yTemp );
	SetVisibleDataMap( xTemp, yTemp, iconExit );
}
void Room::SetOuterWalls( )
{
	for( int y = 0; y < GetLength( ); y++ )
	{
		for( int x = 0; x < GetWidth( ); x++ )
		{
			if( GetVisibleDataMap( x, y ) == '=' )
			{
				continue;
			}
			else if( x == GetWidth( ) - 1 ||
					 y == GetLength( ) - 1 ||
					 x == 0 ||
					 y == 0 )
			{
				wall.emplace_back( x, y );
				SetVisibleDataMap( x, y, iconWall );
			}
		}
	}
}
void Room::SetInvisiblePath( )
{
	// Path from player starting point to exit.
	// Walls cannot be placed on path.
	// Player can no longer be cut off from exit due to randomly palced walls.
	// Function can be replaced by A* algorithm, or similar algorithm.

	int xNew = player.xPosition;
	int yNew = player.yPosition;
	int xPrev;
	int yPrev;
	int random;
	double high = sqrt( GetLength( ) * GetWidth( ) ) * 3;
	double low = sqrt( GetLength( ) * GetWidth( ) ) * 2;
	int threshold = RandomNumberGenerator( ( int ) low, ( int ) high );
	int thresholdCounter = 0;

	if( path.size( ) == 0 )
	{
		path.emplace_back( player.xPosition, player.yPosition );
		SetHiddenDataMap( player.xPosition, player.yPosition, iconPath );
	}

	while( true )
	{
		xPrev = xNew;
		yPrev = yNew;
		random = RandomNumberGenerator( 1, 16 );

		if( thresholdCounter < threshold ) // Equal chance to move in any direction.
		{
			if( random <= 4 )
			{
				xNew++;
			}
			else if( random <= 8 )
			{
				yNew++;
			}
			else if( random <= 12 )
			{
				xNew--;
			}
			else
			{
				yNew--;
			}

			thresholdCounter++;
		}
		else // Increased chance to move right and down.
		{
			if( random <= 6 )
			{
				xNew++;
			}
			else if( random <= 14 )
			{
				yNew++;
			}
			else if( random <= 15 )
			{
				xNew--;
			}
			else
			{
				yNew--;
			}
		}

		if( GetVisibleDataMap( xNew, yNew ) == iconWall )
		{
			xNew = xPrev;
			yNew = yPrev;
		}
		else
		{
			path.emplace_back( xNew, yNew );
			SetHiddenDataMap( xNew, yNew, iconPath );
		}

		if( path.back( ).xPosition == exit.back( ).xPosition &&
			path.back( ).yPosition == exit.back( ).yPosition )
		{
			break;
		}
	}
}
void Room::SetRandomWalls( )
{
	int xTemp;
	int yTemp;
	int infiniteLoopBreaker = 0;

	double high_0 = sqrt( GetLength( ) * GetWidth( ) ) * 1.5;
	double low_0 = sqrt( GetLength( ) * GetWidth( ) ) / 1.5;
	int randomSourceWalls = RandomNumberGenerator( ( int ) low_0, ( int ) high_0 );
	int randomTries = RandomNumberGenerator( 5, 10 );

	while( randomSourceWalls > 0 ) // Place source walls.
	{
		for( int y = 0; y < GetLength( ); y++ )
		{
			for( int x = 0; x < GetWidth( ); x++ )
			{
				if( GetVisibleDataMap( x, y ) == '-' &&
					GetVisibleDataMap( x, y ) != iconWall &&
					GetHiddenDataMap( x, y ) != iconPath &&
					RandomNumberGenerator( 1, 1000 ) == 1 )
				{
					wall.emplace_back( x, y );
					SetVisibleDataMap( x, y, iconWall );
					randomSourceWalls--;

					std::cout << ".";
				}
			}
		}

		if( infiniteLoopBreaker++ > 10000 )
		{
			break;
		}
	}

	std::cout << "\n\nLoading walls.\n";

	for( int i = 0; i < randomTries; i++ ) // Place extension walls.
	{
		std::cout << ".";

		for( int y = 1; y < GetLength( ) - 1; y++ )
		{
			for( int x = 1; x < GetWidth( ) - 1; x++ )
			{
				if( GetVisibleDataMap( x, y ) == iconWall )
				{
					xTemp = x + RandomNumberGenerator( 0, 1 ) * RandomPositiveNegativeGenerator( );
					yTemp = y + RandomNumberGenerator( 0, 1 ) * RandomPositiveNegativeGenerator( );

					if( GetVisibleDataMap( xTemp, yTemp ) == '-' &&
						GetHiddenDataMap( xTemp, yTemp ) != iconPath )
					{
						wall.emplace_back( xTemp, yTemp );
						SetVisibleDataMap( xTemp, yTemp, iconWall );
					}
				}
				else if( GetVisibleDataMap( x, y ) == '-' &&
						 GetHiddenDataMap( x, y ) != iconPath &&
						 CheckEmptySurroundedTile( x, y ) == true )
				{
					wall.emplace_back( x, y );
					SetVisibleDataMap( x, y, iconWall );
				}
			}
		}
	}

	std::cout << "\n";
}
void Room::SetRandomMonsterPositions( )
{
	int xTemp;
	int yTemp;
	int infiniteLoopBreaker = 0;

	for( unsigned int i = 0; i < monster.size( ); i++ )
	{
		std::cout << ".";

		do
		{
			xTemp = RandomNumberGenerator( 1, GetWidth( ) - 2 );
			yTemp = RandomNumberGenerator( 1, GetLength( ) - 2 );

			if( infiniteLoopBreaker++ > 10000 )
			{
				break;
			}
		}
		while( GetVisibleDataMap( xTemp, yTemp ) != '-' );

		monster[i].xPosition = xTemp;
		monster[i].yPosition = yTemp;
		SetVisibleDataMap( xTemp, yTemp, iconMonster );
	}
}

void Room::DrawRoom( )
{
	for( int y = 0; y < GetLength( ); y++ )
	{
		for( int x = 0; x < GetWidth( ); x++ )
		{
			if( GetFogOfWarDataMap( x, y ) == 1 )
			{
				std::cout << GetVisibleDataMap( x, y );
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
	int xTemp;
	int yTemp;
	std::string userChoice;

	std::cout << "[W] Go up\n";
	std::cout << "[S] Go down\n";
	std::cout << "[A] Go left\n";
	std::cout << "[D] Go Right\n";
	std::cout << "[Q] Do nothing\n";
	std::cout << "[E] Exit to meny\n";

	RETRY:
	std::cout << "\nYour choice: ";
	std::cin >> userChoice;

	switch( userChoice[0] )
	{
		case 'W':
		case 'w':       // Move up.
		{
			xTemp = player.xPosition;
			yTemp = player.yPosition - 1;

			break;
		}

		case 'S':
		case 's':       // Move down.
		{
			xTemp = player.xPosition;
			yTemp = player.yPosition + 1;

			break;
		}

		case 'A':
		case 'a':       // Move left.
		{
			xTemp = player.xPosition - 1;
			yTemp = player.yPosition;

			break;
		}

		case 'D':
		case 'd':       // Move right.
		{
			xTemp = player.xPosition + 1;
			yTemp = player.yPosition;

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
			goto RETRY;
		}
	}

	if( GetVisibleDataMap( xTemp, yTemp ) != iconWall )
	{
		SetVisibleDataMap( player.xPosition, player.yPosition, '-' );
		player.xPosition = xTemp;
		player.yPosition = yTemp;
		SetVisibleDataMap( player.xPosition, player.yPosition, iconPlayer );
	}

	return 0;
}
void Room::RandomizeMonsterMovement( )
{
	int xTemp;
	int yTemp;
	int randomNumber;

	for( unsigned int i = 0; i < monster.size( ); i++ )
	{
		while( true )
		{
			randomNumber = RandomNumberGenerator( 1, 8 );	// 25% to move, 75% to stand still.

			if( randomNumber == 1 )							// Monster moves vertically.
			{
				xTemp = monster[i].xPosition;
				yTemp = monster[i].yPosition + RandomPositiveNegativeGenerator( );
			}
			else if( randomNumber == 2 )					// Monster moves horizontally.
			{
				xTemp = monster[i].xPosition + RandomPositiveNegativeGenerator( );
				yTemp = monster[i].yPosition;
			}
			else											// Monster stands still.
			{
				break;
			}

			if( GetVisibleDataMap( xTemp, yTemp ) == '-' ||
				GetVisibleDataMap( xTemp, yTemp ) == iconPlayer )
			{
				int xTemp_0 = monster[i].xPosition;
				int yTemp_0 = monster[i].yPosition;

				if( CheckPosition( xTemp_0, yTemp_0, player ) == false )	// Player movement is executed before monster movement, so
				{															// clearing the monster's previous position after the player
					SetVisibleDataMap( xTemp_0, yTemp_0, '-' );				// has moved to that position will make the player icon invisible.
				}															// (this check avoids that)

				monster[i].xPosition = xTemp;
				monster[i].yPosition = yTemp;
				SetVisibleDataMap( xTemp, yTemp, iconMonster );
				break;
			}
		}
	}
}

bool Room::CheckWinCondition( )
{
	for( unsigned int i = 0; i < exit.size( ); i++ )
	{
		if( CheckPosition( player, exit[i] ) == true )
		{
			return true;
		}
	}

	return false;
}
bool Room::CheckLoseCondition( )
{
	for( unsigned int i = 0; i < monster.size( ); i++ )
	{
		if( CheckPosition( player, monster[i] ) == true )
		{
			return true;
		}
	}

	return false;
}