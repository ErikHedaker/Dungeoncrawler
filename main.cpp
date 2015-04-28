#include "Room.h"
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <math.h>

char Player::icon = '@';
char Monster::icon = 'M';
char Wall::icon = '#';
char Exit::icon = '=';

// Functions used by functions.
int getPositiveInteger( )
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
int randomNumberGenerator( int min, int max )
{
	static std::random_device randomEngine;
	std::mt19937 generator( randomEngine( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}
int randomPositiveNegativeGenerator( )
{
	int randomNumber = randomNumberGenerator( 0, 1 );

	if( randomNumber == 1 )
	{
		return 1;
	}
	else
	{
		return -1;
	}
}
bool checkEmptySurroundedTile( Room &room, int xCurrent, int yCurrent )
{
	int surroundingWalls = 0;

	for( int y = -1; y <= 1; y++ )
	{
		for( int x = -1; x <= 1; x++ )
		{
			if( xCurrent == room.width( ) - 1 ||
				yCurrent == room.length( ) - 1 ||
				xCurrent == 0 ||
				yCurrent == 0 )
			{
				continue;
			}
			else if( room.checkPosition( xCurrent, yCurrent, room.wall ) == true )
			{
				surroundingWalls++;
			}
		}
	}

	if( surroundingWalls > 4 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Default gamestate.
void printGameRules( )
{
	std::cout << "Win condition: Enter the exit.\n";
	std::cout << "Lose condition: Enter the same space as a monster.\n";
}
void setRoomSize( Room &room )
{
	std::string userChoice;

	std::cout << "\nWould you like to determine the room dimensions, [y/n]: ";
	while( true )
	{
		std::cin >> userChoice;

		if( userChoice[0] == 'Y' || userChoice[0] == 'y' )
		{
			std::cout << "Enter the playing Room length: ";
			room.lengthSet( getPositiveInteger( ) );
			std::cout << "Enter the playing Room width: ";
			room.widthSet( getPositiveInteger( ) );
			break;
		}
		else if( userChoice[0] == 'N' || userChoice[0] == 'n' )
		{
			room.lengthSet( randomNumberGenerator( 15, 30 ) );
			room.widthSet( randomNumberGenerator( 50, 100 ) );
			break;
		}
		else
		{
			std::cout << "Invalid input, try again: ";
		}
	}
}
void setRoomExits( Room &room )
{
	Exit tempExit;
	room.exit.push_back( tempExit );

	int x = room.width( ) - 2;
	int y = room.length( ) - 1;
	room.xSet( room.exit.back( ), x );	// Default.
	room.ySet( room.exit.back( ), y );	// bottom right corner.
}
void setRoomOuterWalls( Room &room )
{
	Wall tempWall;

	for( int y = 0; y < room.length( ); y++ )
	{
		for( int x = 0; x < room.width( ); x++ )
		{
			if( room.checkPosition( x, y, room.exit ) == true )
			{
				continue;
			}
			else if( x == room.width( ) - 1 ||
					 y == room.length( ) - 1 ||
					 x == 0 ||
					 y == 0 )
			{
				room.wall.push_back( tempWall );
				room.xSet( room.wall.back( ), x );
				room.ySet( room.wall.back( ), y );
				room.outerWallsAmountIncrease( );
			}
		}
	}
}
void setRoomRandomWalls( Room &room )
{
	Wall tempWall;
	int xTemp;
	int yTemp;
	double high = sqrt( room.length( ) * room.width( ) ) * 1.5;
	double low = sqrt( room.length( ) * room.width( ) ) / 1.5;
	int randomSourceWalls = randomNumberGenerator( (int)low, (int)high );
	int randomTries = randomNumberGenerator( 5, 7 );

	while( randomSourceWalls > 0 ) // Place source walls.
	{
		for( int y = 0; y < room.length( ); y++ )
		{
			for( int x = 0; x < room.width( ); x++ )
			{
				if( room.checkPosition( x, y, room.wall ) == false &&
					randomNumberGenerator( 1, 1000 ) == 1 )
				{
					room.wall.push_back( tempWall );
					room.xSet( room.wall.back( ), x );
					room.ySet( room.wall.back( ), y );
					randomSourceWalls--;
				}
			}
		}
	}

	std::cout << "\nLoading, please wait.\n";

	for( int i = 0; i < randomTries; i++ ) // Place extension walls.
	{
		std::cout << ".";

		for( int y = 0; y < room.length( ); y++ )
		{
			for( int x = 0; x < room.width( ); x++ )
			{
				if( room.checkPosition( x, y, room.wall, room.outerWallsAmount( ) ) == true )	// Check for source walls, excluding outer walls.
				{
					xTemp = x + randomNumberGenerator( 0, 1 ) * randomPositiveNegativeGenerator( );
					yTemp = y + randomNumberGenerator( 0, 1 ) * randomPositiveNegativeGenerator( );

					if( room.checkPosition( xTemp, yTemp, room.wall ) == true ||
						room.checkProtectRange( room.player, xTemp, yTemp ) == true ||
						room.checkProtectRange( room.exit, xTemp, yTemp ) == true )
					{
						continue;
					}
					else
					{
						room.wall.push_back( tempWall );
						room.xSet( room.wall.back( ), xTemp );
						room.ySet( room.wall.back( ), yTemp );
					}
				}
				else if( checkEmptySurroundedTile( room, x, y ) == true )
				{
					room.wall.push_back( tempWall );
					room.xSet( room.wall.back( ), x );
					room.ySet( room.wall.back( ), y );
				}
			}
		}
	}
	
	std::cout << "\n";
}
void setPlayerPosition( Room &room )
{
	room.xSet( room.player, 1 );
	room.ySet( room.player, 1 );   // Top left corner.
}
void chooseMonsterAmount( Room &room )
{
	int maxMonsters = ( room.length( ) * room.width( ) ) - room.wall.size( ) - ( ( 5 * 5 ) * 2 );
																			   // From checkProtectRange( ) of player and exit.
	std::cout << "\nEnter amount of monsters: ";
	while( true )
	{
		room.monsterAmountDesiredSet( getPositiveInteger( ) );

		if( room.monsterAmountDesired( ) >= maxMonsters &&
			room.monsterAmountDesired( ) != 0 )
		{
			std::cout << "Too many monsters, try again: ";
			continue;
		}
		else
		{
			break;
		}
	}
}
void setRandomMonsterPositions( Room &room )
{
	Monster tempMonster;
	int xTemp;
	int yTemp;

	for( int i = 0; i < room.monsterAmountDesired( ); i++ )
	{
		room.monster.push_back( tempMonster );

		while( true )
		{
			xTemp = randomNumberGenerator( 1, room.width( ) - 2 );
			yTemp = randomNumberGenerator( 1, room.length( ) - 2 );

			if( room.checkProtectRange( room.player, xTemp, yTemp ) == false &&
				room.checkPosition( xTemp, yTemp, room.monster ) == false &&
				room.staticDataMap( xTemp, yTemp ) == '-' )
			{
				break;
			}
		}

		room.xSet( room.monster.back( ), xTemp );
		room.ySet( room.monster.back( ), yTemp );
	}
}

// Gameloop.
void drawRoom( Room &room )
{
	for( int y = 0; y < room.length( ); y++ )
	{
		for( int x = 0; x < room.width( ); x++ )
		{
			std::cout << room.completeDataMap( x, y );
		}

		std::cout << "\n";
	}

	std::cout << "\n";
}
void sayTurnOptions( )
{
	std::cout << "[W] Go up.\n";
	std::cout << "[S] Go down.\n";
	std::cout << "[A] Go left.\n";
	std::cout << "[D] Go Right.\n";
	std::cout << "[Q] Do nothing.\n";
	std::cout << "[E] Exit game.\n\n";
}
void chooseTurnOptions( Room &room )
{
	int xTemp;
	int yTemp;
	std::string userChoice;

	RETRY:
	std::cout << "Your choice: ";
	std::cin >> userChoice;

	switch( userChoice[0] )
	{
		case 'W':
		case 'w':       // Move up.
		{
			xTemp = room.x( room.player );
			yTemp = room.y( room.player ) - 1;

			break;
		}

		case 'S':
		case 's':       // Move down.
		{
			xTemp = room.x( room.player );
			yTemp = room.y( room.player ) + 1;

			break;
		}

		case 'A':
		case 'a':       // Move left.
		{
			xTemp = room.x( room.player ) - 1;
			yTemp = room.y( room.player );

			break;
		}

		case 'D':
		case 'd':       // Move right.
		{
			xTemp = room.x( room.player ) + 1;
			yTemp = room.y( room.player );

			break;
		}

		case 'Q':
		case 'q':       // Do nothing.
		{
			return;
		}

		case 'E':
		case 'e':       // Exit game.
		{
			exit( 0 );
		}

		default:
		{
			std::cout << "Invalid input, try again.\n";
			goto RETRY;
		}
	}

	if( room.completeDataMap( xTemp, yTemp ) != '#' )	// Faster than comparing the position of every wall.
	{
		room.xSet( room.player, xTemp );
		room.ySet( room.player, yTemp );
	}
}
void randomizeMonsterMovement( Room &room )
{
	int xTemp;
	int yTemp;
	int randomNumber;

	for( unsigned int i = 0; i < room.monster.size( ); i++ )
	{
		while( true )
		{
			randomNumber = randomNumberGenerator( 1, 16 ); // 25% to move, 75% to stand still.

			if( randomNumber <= 2 )	// Monster moves vertically.
			{
				xTemp = room.x( room.monster[i] );
				yTemp = room.y( room.monster[i] ) + randomPositiveNegativeGenerator( );
			}
			else if( randomNumber <= 4 ) // Monster moves horizontally.
			{
				xTemp = room.x( room.monster[i] ) + randomPositiveNegativeGenerator( );
				yTemp = room.y( room.monster[i] );
			}
			else // Monster stands still.
			{
				break;
			}

			if( room.completeDataMap( xTemp, yTemp ) == '-' ||
				room.completeDataMap( xTemp, yTemp ) == 'P' )
			{
				room.xSet( room.monster[i], xTemp );
				room.ySet( room.monster[i], yTemp );
				break;
			}
		}
	}
}

// Checked during or after gameloop.
bool checkWinCondition(  Room &room )
{
	if( room.checkPosition( room.player, room.exit ) == true )
	{
		std::cout << "\nYou win!";
		return true;
	}

	return false;
}
bool checkLoseCondition( Room &room )
{
	if( room.checkPosition( room.player, room.monster ) == true )
	{
		std::cout << "\nYou lose!";
		return true;
	}

	return false;
}
bool chooseRestartGame( )
{
	std::string userChoice;

	std::cout << "\n\nDo you wish to restart the game, [y/n]: ";
	while( true )
	{
		std::cin >> userChoice;

		if( userChoice[0] == 'Y' || userChoice[0] == 'y' )
		{
			return true;
		}
		else if( userChoice[0] == 'N' || userChoice[0] == 'n' )
		{
			return false;
		}
		else
		{
			std::cout << "Invalid input, try again: ";
		}
	}
}

int main( )
{
	do
	{
		std::vector<Room> room;
		room.resize( 5 );
		int i = 0;

		system( "CLS" );
		printGameRules( );
		setRoomSize( room[i] );
		setRoomExits( room[i] );
		setRoomOuterWalls( room[i] );
		setRoomRandomWalls( room[i] );
		room[i].staticDataMap( );
		setPlayerPosition( room[i] );
		chooseMonsterAmount( room[i] );
		setRandomMonsterPositions( room[i] );

		while( true )
		{
			system( "CLS" );
			room[i].completeDataMap( );
			drawRoom( room[i] );
			if( checkWinCondition( room[i] ) == true ||
				checkLoseCondition( room[i] ) == true )
			{
				break;
			}
			sayTurnOptions( );
			chooseTurnOptions( room[i] );
			randomizeMonsterMovement( room[i] );
		}
	}
	while( chooseRestartGame( ) == true );

	return 0;
}