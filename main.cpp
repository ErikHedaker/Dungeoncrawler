#include "Room.h"
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <math.h>

int GetPositiveInteger( )
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
int RandomNumberGenerator( int min, int max )
{
	static std::random_device randomEngine;
	std::mt19937 generator( randomEngine( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}
int RandomPositiveNegativeGenerator( )
{
	int randomNumber;

	do
	{
		randomNumber = RandomNumberGenerator( -1, 1 );
	}
	while( randomNumber == 0 );

	return randomNumber;
}
bool EmptySurroundedTile( Room &room, Position &current )
{
	int surroundingWalls = 0;

	for( int y = -1; y <= 1; y++ )
	{
		for( int x = -1; x <= 1; x++ )
		{
			if( current.x == 0 || current.x == room.width( ) - 1 ||
				current.y == 0 || current.y == room.length( ) - 1 )
			{
				continue;
			}
			else if( room.checkPosition( current, room.wall ) == true )
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

void PrintGameRules( )
{
	std::cout << "Win  condition: Enter the exit.\n";
	std::cout << "Lose condition: Enter the same space as a monster.\n";
}
void SetRoomSize( Room &room )
{
	std::string userChoice;

	std::cout << "\nWould you like to determine the room dimensions, [y/n]: ";
	while( true )
	{
		std::cin >> userChoice;

		if( userChoice[0] == 'Y' || userChoice[0] == 'y' )
		{
			std::cout << "Enter the playing Room length: ";
			room.lengthSet( GetPositiveInteger( ) );
			std::cout << "Enter the playing Room width: ";
			room.widthSet( GetPositiveInteger( ) );
			break;
		}
		else if( userChoice[0] == 'N' || userChoice[0] == 'n' )
		{
			room.lengthSet( RandomNumberGenerator( 15, 30 ) );
			room.widthSet( RandomNumberGenerator( 50, 100 ) );
			break;
		}
		else
		{
			std::cout << "Invalid input, try again: ";
		}
	}
}
void SetRoomExits( Room &room )
{
	Exit temp;
	room.exit.push_back( temp );
	room.xSet( room.exit[0], room.width( ) - 2 );  // Default.
	room.ySet( room.exit[0], room.length( ) - 1 ); // bottom right corner.
}
void SetRoomOuterWalls( Room &room )
{
	Position tempPosition;
	Wall tempWall;

	for( int y = 0; y < room.length( ); y++ )
	{
		for( int x = 0; x < room.width( ); x++ )
		{
			tempPosition.x = x;
			tempPosition.y = y;
			if( room.checkPosition( tempPosition, room.exit ) == true )
			{
				continue;
			}
			else if( x == 0 || x == room.width( ) - 1 ||
					 y == 0 || y == room.length( ) - 1 )
			{
				room.wall.push_back( tempWall );
				room.xSet( room.wall, x );
				room.ySet( room.wall, y );
			}
		}
	}
}
void SetRoomRandomWalls( Room &room )
{
	Position tempPosition;
	Wall tempWall;
	double high = sqrt( room.length( ) * room.width( ) ) * 1.5;
	double low = sqrt( room.length( ) * room.width( ) ) / 1.5;
	int randomSourceWalls = RandomNumberGenerator( (int)low, (int)high );
	int randomTries = RandomNumberGenerator( 5, 7 );

	while( randomSourceWalls > 0 ) // Place source walls.
	{
		for( int y = 0; y < room.length( ); y++ )
		{
			for( int x = 0; x < room.width( ); x++ )
			{
				tempPosition.x = x;
				tempPosition.y = y;
				if( room.checkPosition( tempPosition, room.wall ) == false &&
					RandomNumberGenerator( 1, 1000 ) == 1 )
				{
					room.wall.push_back( tempWall );
					room.xSet( room.wall, x );
					room.ySet( room.wall, y );
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
				tempPosition.x = x;
				tempPosition.y = y;

				if( room.checkPosition( tempPosition, room.wall ) == true )
				{
					tempPosition.x = x + RandomPositiveNegativeGenerator( ) * RandomNumberGenerator( 0, 1 );
					tempPosition.y = y + RandomPositiveNegativeGenerator( ) * RandomNumberGenerator( 0, 1 );

					if( room.checkPosition( tempPosition, room.wall ) == true ||
						room.checkProtectRange( room.player, tempPosition ) == true ||
						room.checkProtectRange( room.player, tempPosition ) == true )
					{
						continue;
					}
					else
					{
						room.wall.push_back( tempWall );
						room.xSet( room.wall, tempPosition.x );
						room.ySet( room.wall, tempPosition.y );
					}
				}
				else if( EmptySurroundedTile( room, tempPosition ) == true )
				{
					room.wall.push_back( tempWall );
					room.xSet( room.wall, x );
					room.ySet( room.wall, y );
				}
			}
		}
	}

	std::cout << std::endl;
}
void SetPlayerPosition( Room &room )
{
	room.xSet( room.player, 1 );
	room.ySet( room.player, 1 );   // Top left corner.
}
void ChooseMonsterAmount( Room &room )
{
	int maxMonsters = ( room.length( ) * room.width( ) ) - room.wall.size( );

	std::cout << "\nEnter amount of monsters: ";
	while( true )
	{
		room.monsterAmountDesiredSet( GetPositiveInteger( ) );

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
void SetRandomMonsterPositions( Room &room )
{
	Position tempPosition;
	Monster tempMonster;

	for( int i = 0; i < room.monsterAmountDesired( ); i++ )
	{
		while( true )
		{
			tempPosition.x = RandomNumberGenerator( 1, room.width( ) - 2 );
			tempPosition.y = RandomNumberGenerator( 1, room.length( ) - 2 );

			if( room.checkProtectRange( room.player, room.monster[i] ) == false &&
				room.checkPosition( tempPosition, room.monster ) == false &&
				room.staticDataMap( tempPosition.x, tempPosition.y ) == '-' )
			{
				break;
			}
		}

		//if(room.monster.size( ) < room.monsterAmountDesired( ) )
		//{
		//	Instance.Unit.Monster.amountCurrent++;
		//}	// To avoid trying to access unvalid memory of monsterpositions
		//	// before its allocated inside CheckForMonsterPosition( ).

		room.monster.push_back( tempMonster );
		room.xSet( room.monster[i], tempPosition.x );
		room.ySet( room.monster[i], tempPosition.y );
	}
}

void DrawRoom( Room &room )
{
	for( int y = 0; y < room.length( ); y++ )
	{
		for( int x = 0; x < room.width( ); x++ )
		{
			std::cout << room.completeDataMap( x, y );
		}

		std::cout << "\n";
	}

	std::cout << std::endl;
}
void SayTurnOptions( )
{
	std::cout << "[W] Go up.\n";
	std::cout << "[S] Go down.\n";
	std::cout << "[A] Go left.\n";
	std::cout << "[D] Go Right.\n";
	std::cout << "[Q] Do nothing.\n";
	std::cout << "[E] Exit game.\n\n";
}
void ChooseTurnOptions( Room &room )
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
			std::cout << "Invalid input, try again\n";
			goto RETRY;
		}
	}

	if( room.completeDataMap( xTemp, yTemp ) != '#' )
	{
		room.xSet( room.player, xTemp );
		room.ySet( room.player, yTemp );
	}
}
void RandomizeMonsterMovement( Room &room )
{
	int xTemp;
	int yTemp;
	int randomNumber;

	for( unsigned int i = 0; i < room.monster.size( ); i++ )
	{
		while( true )
		{
			randomNumber = RandomNumberGenerator( 1, 16 ); // 25% to move, 75% to stand still.

			if( randomNumber <= 2 ) // Monster moves vertically.
			{
				xTemp = room.x( room.monster[i] );
				yTemp = room.y( room.monster[i] ) + RandomPositiveNegativeGenerator( );
			}
			else if( randomNumber <= 4 ) // Monster moves horizontally.
			{
				xTemp = room.x( room.monster[i] ) + RandomPositiveNegativeGenerator( );
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

bool ChooseRestartGame( )
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
		PrintGameRules( );
		SetRoomSize( room[i] );
		SetRoomExits( room[i] );
		SetRoomOuterWalls( room[i] );
		SetRoomRandomWalls( room[i] );
		room[i].staticDataMap( );
		SetPlayerPosition( room[i] );
		ChooseMonsterAmount( room[i] );
		SetRandomMonsterPositions( room[i] );

		while( true )
		{
			system( "CLS" );
			room[i].completeDataMap( );
			DrawRoom( room[i] );
			if( checkWinCondition( room[i] ) == true ||
				checkLoseCondition( room[i] ) == true )
			{
				break;
			}
			SayTurnOptions( );
			ChooseTurnOptions( room[i] );
			RandomizeMonsterMovement( room[i] );
		}
	}
	while( ChooseRestartGame( ) == true );

	return 0;
}