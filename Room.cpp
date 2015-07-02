#include "Room.h"
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <math.h>

extern const char iconPlayer;
extern const char iconMonster;
extern const char iconWall;
extern const char iconExit;
extern const char iconPath;

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
bool Room::CheckPosition( Entity& current, std::vector<Entity>& search )
{
	for( unsigned int i = 0; i < search.size( ); i++ )
	{
		if( current.xPosition == search[i].xPosition &&
			current.yPosition == search[i].yPosition )
		{
			return true;
		}
	}

	return false;
}
bool Room::CheckPosition( int xCurrent, int yCurrent, std::vector<Entity>& search )
{
	for( unsigned int i = 0; i < search.size( ); i++ )
	{
		if( xCurrent == search[i].xPosition &&
			yCurrent == search[i].yPosition )
		{
			return true;
		}
	}

	return false;
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
	_hiddenDataMap.resize( _length, std::vector<char>( _width, '\0' ) );

	for( int y = 0; y < _length; y++ )
	{
		for( int x = 0; x < _width; x++ )
		{
			for( unsigned int i = 0; i < path.size( ); i++ )
			{
				if( CheckPosition( x, y, path[i] ) == true )
				{
					_hiddenDataMap[y][x] = iconPath;
				}
			}
		}
	}
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

	for( int y = 0; y < _length; y++ )
	{
		for( int x = 0; x < _width; x++ )
		{
			if( CheckPosition( x, y, exit ) == true )
			{
				_visibleDataMap[y][x] = iconExit;		// '='
			}
			else if( CheckPosition( x, y, wall ) == true )
			{
				_visibleDataMap[y][x] = iconWall;		// '#'
			}
			else if( CheckPosition( x, y, monster ) == true )
			{
				_visibleDataMap[y][x] = iconMonster;	// 'M'
			}
			else if( CheckPosition( x, y, player ) == true )
			{
				_visibleDataMap[y][x] = iconPlayer;		// '@'
			}
		}
	}
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

//void Room::visibleDataMapFogOfWarLineOfSightBuild( int range )
//{
//	// Not in use due to bugs.
//
//	_visibleDataMapFogOfWar.clear( );
//	_visibleDataMapFogOfWar.resize( length( ), std::vector<char>( width( ) ) );
//	_visibleDataMapFogOfWar.assign( length( ), std::vector<char>( width( ), 0 ) );
//
//	for( int y = player.y; y < player.y + range; y++ )
//	{
//		if( y > length( ) || y < 0 )
//		{
//			break;
//		}
//
//		for( int x = player.x; x < player.x + range; x++ )
//		{
//			if( x > width( ) || x < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_0;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_0:
//
//	for( int y = player.y; y > player.y - range; y-- )
//	{
//		if( y > length( ) || y < 0 )
//		{
//			break;
//		}
//
//		for( int x = player.x; x < player.x + range; x++ )
//		{
//			if( x > width( ) || x < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_1;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_1:
//
//	for( int y = player.y; y < player.y + range; y++ )
//	{
//		if( y > length( ) || y < 0 )
//		{
//			break;
//		}
//
//		for( int x = player.x; x > player.x - range; x-- )
//		{
//			if( x > width( ) || x < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_2;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_2:
//
//	for( int y = player.y; y > player.y - range; y-- )
//	{
//		if( y > length( ) || y < 0 )
//		{
//			break;
//		}
//
//		for( int x = player.x; x > player.x - range; x-- )
//		{
//			if( x > width( ) || x < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_3;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_3:
//
//	// Check again, this time y and x loops are reversed.
//	// This should fix any visual bugs.
//
//	for( int x = player.x; x < player.x + range; x++ )
//	{
//		if( x > width( ) || x < 0 )
//		{
//			break;
//		}
//
//		for( int y = player.y; y < player.y + range; y++ )
//		{
//			if( y > length( ) || y < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_4;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_4:
//
//	for( int x = player.x; x > player.x - range; x-- )
//	{
//		if( x > width( ) || x < 0 )
//		{
//			break;
//		}
//
//		for( int y = player.y; y < player.y + range; y++ )
//		{
//			if( y > length( ) || y < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_5;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_5:
//
//	for( int x = player.x; x < player.x + range; x++ )
//	{
//		if( x > width( ) || x < 0 )
//		{
//			break;
//		}
//
//		for( int y = player.y; y > player.y - range; y-- )
//		{
//			if( y > length( ) || y < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_6;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_6:
//
//	for( int x = player.x; x > player.x - range; x-- )
//	{
//		if( x > width( ) || x < 0 )
//		{
//			break;
//		}
//
//		for( int y = player.y; y > player.y - range; y-- )
//		{
//			if( y > length( ) || y < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//return;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//}

// Functions used by functions.
int Room::GetPositiveInteger( )
{
	std::string userChoice;

	RETRY:
	std::cin >> userChoice;

	if( userChoice.size( ) > 9 ) // Manual max int input: 999 999 999, roughly half of the max size for signed int.
	{
		std::cout << "Input is too big, try again: ";
		goto RETRY;
	}

	for( unsigned int i = 0; i < userChoice.size( ); i++ )
	{
		if( isdigit( userChoice[i] ) == false )
		{
			std::cout << "Input needs to consist of digits only, try again: ";
			goto RETRY;
		}
	}

	int output = atoi( userChoice.c_str( ) );

	return output;
}
int Room::RandomNumberGenerator( int min, int max )
{
	static std::random_device randomEngine;
	std::mt19937 generator( randomEngine( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}
int Room::RandomPositiveNegativeGenerator( )
{
	return RandomNumberGenerator( 0, 1 ) ? 1 : -1;
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

// Configuration
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
		int minMonsters = ( GetLength( ) * GetWidth( ) ) - wall.size( ) - ( GetLength( ) * GetWidth( ) ) / 2;

		while( true )
		{
			std::cout << "Enter the amount of monsters: ";
			monsterAmountDesired = GetPositiveInteger( );

			if( monsterAmountDesired >= minMonsters &&
				monsterAmountDesired != 0 )
			{
				std::cout << "Too many monsters, try again.\n";
				continue;
			}
			else
			{
				std::cout << "\n";
				break;
			}
		}
	}
	else
	{
		double high = sqrt( GetLength( ) * GetWidth( ) ) / 1.5;
		double low = sqrt( GetLength( ) * GetWidth( ) ) / 3;
		monsterAmountDesired = RandomNumberGenerator( ( int ) low, ( int ) high );
	}
}

// Automatic
void Room::SetPlayerPosition( )
{
	player.xPosition = 1;
	player.yPosition = 1;
	SetVisibleDataMap( player.xPosition, player.yPosition, iconPlayer );
}
void Room::SetExits( )
{
	int xTemp = GetWidth( ) - 2;
	int yTemp = GetLength( ) - 1;
	exit.emplace_back( );
	exit.back( ).xPosition = xTemp;
	exit.back( ).yPosition = yTemp;
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
				wall.emplace_back( );
				wall.back( ).xPosition = x;
				wall.back( ).yPosition = y;
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

	int xTemp = player.xPosition;
	int yTemp = player.yPosition;
	int xTemp_0;
	int yTemp_0;
	int random;
	double high = sqrt( GetLength( ) * GetWidth( ) ) * 3;
	double low = sqrt( GetLength( ) * GetWidth( ) ) * 2;
	int threshold = RandomNumberGenerator( ( int ) low, ( int ) high );
	int thresholdCounter = 0;

	while( true )
	{
		xTemp_0 = xTemp;
		yTemp_0 = yTemp;
		random = RandomNumberGenerator( 1, 16 );

		if( thresholdCounter < threshold ) // Equal chance to move in any direction.
		{
			if( random <= 4 )
			{
				xTemp++;
			}
			else if( random <= 8 )
			{
				yTemp++;
			}
			else if( random <= 12 )
			{
				xTemp--;
			}
			else
			{
				yTemp--;
			}

			thresholdCounter++;
		}
		else // Increased chance to move right and down.
		{
			if( random <= 6 )
			{
				xTemp++;
			}
			else if( random <= 14 )
			{
				yTemp++;
			}
			else if( random <= 15 )
			{
				xTemp--;
			}
			else
			{
				yTemp--;
			}
		}

		if( GetVisibleDataMap( xTemp, yTemp ) == iconWall )
		{
			xTemp = xTemp_0;
			yTemp = yTemp_0;
		}
		else
		{
			if( path.size( ) == 0 )
			{
				path.emplace_back( );
				path.back( ).xPosition = xTemp_0;
				path.back( ).yPosition = yTemp_0;
				SetHiddenDataMap( xTemp, yTemp, iconPath );
			}

			path.emplace_back( );
			path.back( ).xPosition = xTemp;
			path.back( ).yPosition = yTemp;
			SetHiddenDataMap( xTemp, yTemp, iconPath );
		}

		if( path.size( ) > 0 &&
			path.back( ).xPosition == exit.back( ).xPosition &&
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
					GetHiddenDataMap( x, y ) != iconWall &&
					RandomNumberGenerator( 1, 1000 ) == 1 )
				{
					wall.emplace_back( );
					wall.back( ).xPosition = x;
					wall.back( ).yPosition = y;
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

	std::cout << "\n";

	for( int i = 0; i < randomTries; i++ ) // Place extension walls, loop inside of outer walls.
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
						wall.emplace_back( );
						wall.back( ).xPosition = xTemp;
						wall.back( ).yPosition = yTemp;
						SetVisibleDataMap( xTemp, yTemp, iconWall );
					}
				}
				else if( GetVisibleDataMap( x, y ) == '-' &&
						 GetHiddenDataMap( x, y ) != iconPath &&
						 CheckEmptySurroundedTile( x, y ) == true )
				{
					wall.emplace_back( );
					wall.back( ).xPosition = x;
					wall.back( ).yPosition = y;
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

	for( int i = 0; i < monsterAmountDesired; i++ )
	{
		std::cout << ".";

		monster.emplace_back( );

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

		monster.back( ).xPosition = xTemp;
		monster.back( ).yPosition = yTemp;
		SetVisibleDataMap( xTemp, yTemp, iconMonster );
	}
}

// Gameloop
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

// Checked during or after gameloop
bool Room::CheckWinCondition( )
{
	if( CheckPosition( player, exit ) == true )
	{
		return true;
	}

	return false;
}
bool Room::CheckLoseCondition( )
{
	if( CheckPosition( player, monster ) == true )
	{
		return true;
	}

	return false;
}