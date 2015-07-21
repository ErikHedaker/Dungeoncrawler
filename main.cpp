#include "Room.h"
#include <iostream>
#include <string>
#include <random>

extern const char iconPlayer = '@';
extern const char iconMonster = 'M';
extern const char iconWall = '#';
extern const char iconExit = '=';
extern const char iconPath = ':';
extern const char iconFloor = '-';

extern int GetPositiveInteger( )
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

void PrintGameRules( )
{
	std::cout << "Win condition: Enter the exit.\n";
	std::cout << "Lose condition: Enter the same space as a monster.\n\n";
}
int SetGameState( )
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
		return 1;
	}
	else if( userChoice[0] == '2' )
	{
		return 2;
	}
	else if( userChoice[0] == '3' )
	{
		std::exit( 0 );
	}
	else
	{
		std::cout << "Invalid input, try again.\n";
		goto RETRY;
	}
}

int main( )
{
	std::vector<Room> room;
	int roomMode;

	MENY:
	room.clear( );
	PrintGameRules( );
	roomMode = SetGameState( );

	for( int i = 0; i < 100; i++ )
	{
		system( "CLS" );

		room.emplace_back( );

		room[i].SetRoomSize( roomMode );
		room[i].SetRoomLineOfSight( roomMode );
		room[i].SetRoomMonsterAmount( roomMode );

		room[i].BuildHiddenDataMap( );
		room[i].BuildVisibleDataMap( );
		room[i].BuildFogOfWarDataMap( );

		room[i].SetPlayerPosition( );
		room[i].SetExits( );
		room[i].SetOuterWalls( );
		room[i].SetInvisiblePath( );
		room[i].SetRandomWalls( );
		room[i].SetRandomMonsterPositions( );

		while( true )
		{
			system( "CLS" );

			room[i].UpdateFogOfWarDataMap( );

			room[i].DrawRoom( );

			if( room[i].CheckWinCondition( ) == true )
			{
				std::cout << "You win!";
				std::cout << "\n\nPress enter to continue: ";
				std::cin.get( );
				std::cin.get( );
				break;
			}
			else if( room[i].CheckLoseCondition( ) == true )
			{
				std::cout << "You lose!";
				std::cout << "\n\nPress enter to continue: ";
				std::cin.get( );
				std::cin.get( );
				break;
			}

			if( room[i].TurnOptions( ) == 'E' )
			{
				goto MENY;
			}

			room[i].RandomizeMonsterMovement( );
		}
	}

	return 0;
}