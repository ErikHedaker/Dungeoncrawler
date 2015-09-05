#include "Room.h"
#include <iostream>
#include <string>
#include <random>

extern int GetPositiveInteger( const std::string& context )
{
	std::string userChoice;

	RETRY:
	std::cout << context;
	std::cin >> userChoice;

	if( userChoice.size( ) > 9 ) // Manual max int input: 999 999 999, roughly half of the max size for signed int.
	{
		std::cout << "\nInput is too big, try again.\n\n";
		goto RETRY;
	}

	for( unsigned int i = 0; i < userChoice.size( ); i++ )
	{
		if( isdigit( userChoice[i] ) == false )
		{
			std::cout << "\nInput needs to consist of digits only, try again.\n\n";
			goto RETRY;
		}
	}

	return atoi( userChoice.c_str( ) );
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
/*
extern std::vector<Vector2i> AStarPathFindningAlgorithm( Vector2i start, Vector2i goal )
{

}
*/

void PrintGameRules( )
{
	std::cout << "Win condition: Enter the exit.\n";
	std::cout << "Lose condition: Enter the same space as a monster.\n\n";
}
int SetGameType( )
{
	std::string userChoice;

	std::cout << "Choose the game type.\n";
	std::cout << "[1] Randomization mode\n";
	std::cout << "[2] Configuration mode\n";
	std::cout << "[3] Exit game\n";

	while( true )
	{
		std::cout << "\nYour choice: ";
		std::cin >> userChoice;

		switch( userChoice[0] )
		{
			case '1':
			{
				return 1;
			}

			case '2':
			{
				return 2;
			}

			case '3':
			{
				std::exit( 0 );
			}

			default:
			{
				std::cout << "\nInvalid input, try again.\n";
				break;
			}
		}
	}
}

void GameLoop( Room& room )
{
	while( true )
	{
		room.UpdateFogOfWarData( );
		room.DrawRoom_Iterator1D( );

		if( room.CheckWinCondition( ) == true ||
			room.CheckLoseCondition( ) == true ||
			room.TurnOptions( ) == 'E' )
		{
			return;
		}

		room.RandomizeMonsterMovement( );
	}
}
void NewGame( int gameType )
{
	Room room;

	system( "CLS" );

	room.SetRoomSize( gameType );
	room.SetRoomLineOfSight( gameType );
	room.SetRoomMonsterAmount( gameType );

	room.BuildHiddenData( );
	room.BuildVisibleData( );
	room.BuildFogOfWarData( );

	room.SetPlayerPosition( );
	room.SetExits( );
	room.SetOuterWalls( );
	room.SetInvisiblePath( );
	room.SetRandomWalls( );
	room.SetRandomMonsterPositions( );

	GameLoop( room );
}

int main( )
{
	while( true )
	{
		system( "CLS" );
		PrintGameRules( );
		NewGame( SetGameType( ) );
	}

	return 0;
}