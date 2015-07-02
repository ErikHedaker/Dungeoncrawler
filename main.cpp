#include "Room.h"
#include <iostream>
#include <string>

extern const char iconPlayer = '@';
extern const char iconMonster = 'M';
extern const char iconWall = '#';
extern const char iconExit = '=';
extern const char iconPath = ':';

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
		room[i].SetMonsterAmount( roomMode );

		std::cout << "Loading, please wait.\n";

		room[i].BuildHiddenDataMap( );
		room[i].BuildVisibleDataMap( );
		room[i].BuildFogOfWarDataMap( );

		room[i].SetPlayerPosition( );
		room[i].SetExits( );
		room[i].SetOuterWalls( );
		room[i].SetInvisiblePath( );
		room[i].SetRandomWalls( );;
		room[i].SetRandomMonsterPositions( );

		while( true )
		{
			system( "CLS" );

			room[i].SetFogOfWarDataMap( );

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