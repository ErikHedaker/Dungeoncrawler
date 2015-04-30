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
char Path::icon = ':';

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

	for( int y = yCurrent - 1; y <= yCurrent + 1; y++ )
	{
		for( int x = xCurrent - 1; x <= xCurrent + 1; x++ )
		{
			if( room.visibleDataMap( x, y ) == Wall::icon )
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

// Meny
void printGameRules( )
{
	std::cout << "Win condition: Enter the exit.\n";
	std::cout << "Lose condition: Enter the same space as a monster.\n\n";
}
void setGameState( int &mode )
{
	std::string userChoice;

	std::cout << "Choose the game state.\n";
	std::cout << "[1] Randomization mode\n";
	std::cout << "[2] Configuration mode\n";
	std::cout << "[3] Exit game\n";

	RETRY:
	std::cout << "\nYour choice: ";
	std::cin >> userChoice;

	if( userChoice[0] == '1' )
	{
		mode = 1;
	}
	else if( userChoice[0] == '2' )
	{
		mode = 2;
	}
	else if( userChoice[0] == '3' )
	{
		exit( 0 );
	}
	else
	{
		std::cout << "Invalid input, try again.\n";
		goto RETRY;
	}
}

// Configuration
void setRoomSize( Room &room, int mode )
{
	if( mode == 2 )
	{
		RETRY:
		std::cout << "Enter the playing Room length: ";
		room.lengthSet( getPositiveInteger( ) );
		std::cout << "Enter the playing Room width: ";
		room.widthSet( getPositiveInteger( ) );
		
		if( room.length( ) < 3 ||
			room.width( ) < 3 )
		{
			std::cout << "\nInput is too small, try again.\n\n";
			goto RETRY;
		}
		else if( room.length( ) > 1000 ||
				 room.width( ) > 1000 )
		{
			std::cout << "\nInput is too big, try again.\n\n";
			goto RETRY;
		}
	}
	else
	{
		room.lengthSet( randomNumberGenerator( 15, 30 ) );
		room.widthSet( randomNumberGenerator( 50, 100 ) );

	}
}
void setLineOfSight( int &LoS, int mode )
{
	if( mode == 2 )
	{
		std::cout << "Enter the line of sight range: ";
		LoS = getPositiveInteger( );
	}
	else
	{
		LoS = 4;
	}
}
void setMonsterAmount( Room &room, int mode )
{
	if( mode == 2 )
	{
		int maxMonsters = ( room.length( ) * room.width( ) ) - room.wall.size( ) - ( room.length( ) * room.width( ) ) / 2;

		while( true )
		{
			std::cout << "Enter the amount of monsters: ";
			room.monsterAmountDesiredSet( getPositiveInteger( ) );

			if( room.monsterAmountDesired( ) >= maxMonsters &&
				room.monsterAmountDesired( ) != 0 )
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
		double high = sqrt( room.length( ) * room.width( ) ) / 1.5;
		double low = sqrt( room.length( ) * room.width( ) ) / 3;
		room.monsterAmountDesiredSet( randomNumberGenerator( ( int ) low, ( int ) high ) );
	}
}

// Automatic
void setPlayerPosition( Room &room )
{
	room.xSet( room.player, 1 );
	room.ySet( room.player, 1 );   // Top left corner.
	room.visibleDataMapUpdate( 1, 1, Player::icon );
}
void setExits( Room &room )
{
	Exit tempExit;
	room.exit.push_back( tempExit );

	int xTemp = room.width( ) - 2;
	int yTemp = room.length( ) - 1;
	room.xSet( room.exit.back( ), xTemp );	// Default.
	room.ySet( room.exit.back( ), yTemp );	// bottom right corner.
	room.visibleDataMapUpdate( xTemp, yTemp, Exit::icon );
}
void setOuterWalls( Room &room )
{
	Wall tempWall;

	for( int y = 0; y < room.length( ); y++ )
	{
		for( int x = 0; x < room.width( ); x++ )
		{
			if( room.visibleDataMap( x, y ) == '=' )
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
				room.visibleDataMapUpdate( x, y, Wall::icon );
				room.outerWallsAmountIncrease( );
			}
		}
	}
}
void setInvisiblePath( Room &room)
{
	// Path from player starting point to exit.
	// Walls cannot be placed on path.
	// Player can no longer be cut off from exit due to randomly palced walls.
	// Function can be replaced by A* algorithm, or similar algorithm.

	Path tempPath;
	int xTemp = room.x( room.player );
	int yTemp = room.y( room.player );
	int xTemp_0;
	int yTemp_0;
	int random;
	int threshold = 50;
	int thresholdCounter = 0;

	while( true )
	{
		xTemp_0 = xTemp;
		yTemp_0 = yTemp;
		random = randomNumberGenerator( 1, 16 );

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

		if( room.visibleDataMap( xTemp, yTemp ) == Wall::icon )
		{
			xTemp = xTemp_0;
			yTemp = yTemp_0;
		}
		else
		{
			if( room.path.size( ) == 0 )
			{
				room.path.push_back( tempPath );
				room.xSet( room.path.back( ), xTemp_0 );
				room.ySet( room.path.back( ), yTemp_0 );
				room.hiddenDataMapUpdate( xTemp, yTemp, Path::icon );
			}

			room.path.push_back( tempPath );
			room.xSet( room.path.back( ), xTemp );
			room.ySet( room.path.back( ), yTemp );
			room.hiddenDataMapUpdate( xTemp, yTemp, Path::icon );
		}

		if( room.path.size( ) > 0 &&
			room.x( room.path.back( ) ) == room.x( room.exit.back( ) ) &&
			room.y( room.path.back( ) ) == room.y( room.exit.back( ) ) )
		{
			break;
		}
	}
}
void setRandomWalls( Room &room )
{
	Wall tempWall;
	int xTemp;
	int yTemp;

	double high_0 = sqrt( room.length( ) * room.width( ) ) * 1.5;
	double low_0 = sqrt( room.length( ) * room.width( ) ) / 1.5;
	int randomSourceWalls = randomNumberGenerator( (int)low_0, (int)high_0 );

	double high_1 = sqrt( room.length( ) * room.width( ) ) / 5;
	double low_1 = sqrt( room.length( ) * room.width( ) ) / 7;
	int randomTries = randomNumberGenerator( (int)low_1, (int)high_1 );

	while( randomSourceWalls > 0 ) // Place source walls.
	{
		for( int y = 0; y < room.length( ); y++ )
		{
			for( int x = 0; x < room.width( ); x++ )
			{
				if( room.visibleDataMap( x, y ) != Wall::icon &&
					room.hiddenDataMap( x, y ) != Path::icon &&
					randomNumberGenerator( 1, 1000 ) == 1 )
				{
					room.wall.push_back( tempWall );
					room.xSet( room.wall.back( ), x );
					room.ySet( room.wall.back( ), y );
					randomSourceWalls--;

					std::cout << ".";
				}
			}
		}
	}

	std::cout << "\n";

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

					if( room.visibleDataMap( xTemp, yTemp ) == '-' &&
						room.hiddenDataMap( xTemp, yTemp ) != Path::icon &&
						room.checkProtectRange( xTemp, yTemp, room.player ) == false &&
						room.checkProtectRange( xTemp, yTemp, room.exit ) == false )
					{
						room.wall.push_back( tempWall );
						room.xSet( room.wall.back( ), xTemp );
						room.ySet( room.wall.back( ), yTemp );
						room.visibleDataMapUpdate( xTemp, yTemp, Wall::icon );
					}
				}
				else if( room.visibleDataMap( x, y ) == '-' &&
						 room.hiddenDataMap( x, y ) != Path::icon &&
						 checkEmptySurroundedTile( room, x, y ) == true )
				{
					room.wall.push_back( tempWall );
					room.xSet( room.wall.back( ), x );
					room.ySet( room.wall.back( ), y );
					room.visibleDataMapUpdate( x, y, Wall::icon );
				}
			}
		}
	}
	
	std::cout << "\n";
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

			if( room.checkProtectRange( xTemp, yTemp, room.player ) == false &&
				room.checkPosition( xTemp, yTemp, room.monster ) == false &&
				room.visibleDataMap( xTemp, yTemp ) == '-' )
			{
				break;
			}
		}

		room.xSet( room.monster.back( ), xTemp );
		room.ySet( room.monster.back( ), yTemp );
		room.visibleDataMapUpdate( xTemp, yTemp, Monster::icon );
	}
}

// Gameloop
void clearScreen( Room &room )
{
	int GAME_WINDOW_SIZE = 40;

	for( int i = 0; i < GAME_WINDOW_SIZE - room.length( ); i++ )
	{
		std::cout << "\n";
	}
}
void drawRoom( Room &room )
{
	for( int y = 0; y < room.length( ); y++ )
	{
		for( int x = 0; x < room.width( ); x++ )
		{
			if( room.visibleDataMapFogOfWar( x, y ) == 1 )
			{
				std::cout << room.visibleDataMap( x, y );
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
char turnOptions( Room &room )
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

	if( room.visibleDataMap( xTemp, yTemp ) != '#' )
	{
		int xTemp_0 = room.x( room.player );
		int yTemp_0 = room.y( room.player );
		room.visibleDataMapUpdate( xTemp_0, yTemp_0, '-' );

		room.xSet( room.player, xTemp );
		room.ySet( room.player, yTemp );
		room.visibleDataMapUpdate( xTemp, yTemp, Player::icon );
	}

	return 0;
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

			if( room.visibleDataMap( xTemp, yTemp ) == '-' ||
				room.visibleDataMap( xTemp, yTemp ) == '@' )
			{
				int xTemp_0 = room.x( room.monster[i] );
				int yTemp_0 = room.y( room.monster[i] );
				room.visibleDataMapUpdate( xTemp_0, yTemp_0, '-' );

				room.xSet( room.monster[i], xTemp );
				room.ySet( room.monster[i], yTemp );
				room.visibleDataMapUpdate( xTemp, yTemp, Monster::icon );
				break;
			}
		}
	}
}

// Checked during or after gameloop
bool checkWinCondition(  Room &room )
{
	if( room.checkPosition( room.player, room.exit ) == true )
	{
		return true;
	}

	return false;
}
bool checkLoseCondition( Room &room )
{
	if( room.checkPosition( room.player, room.monster ) == true )
	{
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
		Room tempRoom;
		std::vector<Room> room;
		int i = -1;
		int mode;
		int LoS;

		MENY:
		printGameRules( );
		setGameState( mode );

		while( true )
		{
			system( "CLS" );
			i++;

			room.push_back( tempRoom );
			setRoomSize( room[i], mode );
			setLineOfSight( LoS, mode );
			setMonsterAmount( room[i], mode );

			room[i].hiddenDataMapBuild( );
			room[i].visibleDataMapBuild( );

			std::cout << "Loading, please wait.\n";
			setPlayerPosition( room[i] );
			setExits( room[i] );
			setOuterWalls( room[i] );
			setInvisiblePath( room[i] );
			setRandomWalls( room[i] );;
			setRandomMonsterPositions( room[i] );

			while( true )
			{
				system( "CLS" );
				room[i].visibleDataMapFogOfWarBuild( LoS );
				drawRoom( room[i] );

				if( checkWinCondition( room[i] ) == true )
				{
					std::cout << "You win!";
					std::cout << "\n\nPress enter to continue: ";
					std::cin.get( );
					std::cin.get( );
					break;
				}
				else if( checkLoseCondition( room[i] ) == true )
				{
					std::cout << "You lose!";
					std::cout << "\n\nPress enter to continue: ";
					std::cin.get( );
					std::cin.get( );
					break;
				}

				if( turnOptions( room[i] ) == 'E' )
				{
					goto MENY;
				}

				randomizeMonsterMovement( room[i] );
			}
		}
	}
	while( chooseRestartGame( ) == true );

	return 0;
}