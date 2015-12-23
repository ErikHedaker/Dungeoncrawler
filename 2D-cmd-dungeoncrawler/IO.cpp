#include "IO.h"
#include "Portrait.h"
#include <iostream>
#include <string>
#include <algorithm>

extern const Vector2i WINDOWSIZE;

void IO::OutputClearScreen( )
{
	/* Some day I will switch to a framework like ncurses and get rid of this abomination */
	system( "CLS" );
}
void IO::OutputStartMessage( )
{
	std::cout << "Win condition: Enter the exit.\n";
	std::cout << "Lose condition: Enter the same space as a monster.\n\n";
	//std::cout << "Increase your console width and height incase the drawing of your dungeon become misaligned.\n\n";
}
void IO::OutputGameTypes( )
{
	std::cout << "Choose the game type.\n\n";
	std::cout << "[1] Configuration mode\n";
	std::cout << "[2] Randomization mode\n";
	std::cout << "[3] Exit game\n";
}
void IO::OutputString( const std::string& string )
{
	/* We can't have all these cout operations sprinkled around like cocaine, now can we? */
	std::cout << string;
}
void IO::OutputDungeonCentered( const Dungeon& dungeon, const Vector2i& dungeonSize, Vector2i positionCenter )
{
	Vector2i cameraIndentation( WINDOWSIZE.col / 5, WINDOWSIZE.row / 5 );
	Vector2i cameraOrigo = positionCenter - cameraIndentation;
	Vector2i iterator;
	Vector2i loopStart( cameraOrigo.col - 1, cameraOrigo.row - 1 );
	Vector2i loopEnd( cameraOrigo.col + cameraIndentation.col + cameraIndentation.col + 1,
					  cameraOrigo.row + cameraIndentation.row + cameraIndentation.row + 1 );

	for( iterator.row = loopStart.row; iterator.row < loopEnd.row; iterator.row++ )
	{
		for( iterator.col = loopStart.col; iterator.col < loopEnd.col; iterator.col++ )
		{
			if( iterator.col == loopStart.col ||
				iterator.row == loopStart.row ||
				iterator.col == loopEnd.col - 1 ||
				iterator.row == loopEnd.row - 1 )
			{
				std::cout << "\\";
			}
			else if( iterator.col >= 0 &&
					 iterator.row >= 0 &&
					 iterator.col <= dungeonSize.col - 1 &&
					 iterator.row <= dungeonSize.row - 1 &&
					 dungeon.GetVisionDataAt( iterator ) == true )
			{
				if( dungeon.GetEntityDataAt( iterator ) != nullptr )
				{
					std::cout << dungeon.GetEntityDataAt( iterator )->portrait;
				}
				else
				{
					std::cout << Portrait::Floor;
				}
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
void IO::OutputDungeon( const Dungeon& dungeon, const Vector2i& dungeonSize )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < dungeonSize.col; iterator.col++ )
		{
			if( dungeon.GetVisionDataAt( iterator ) == true )
			{
				if( dungeon.GetEntityDataAt( iterator ) != nullptr )
				{
					std::cout << dungeon.GetEntityDataAt( iterator )->portrait;
				}
				else
				{
					std::cout << Portrait::Floor;
				}
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
void IO::OutputHiddenDungeon( const Dungeon& dungeon, const Vector2i& dungeonSize )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < dungeonSize.col; iterator.col++ )
		{
			if( dungeon.GetHiddenDataAt( iterator ) != nullptr )
			{
				std::cout << dungeon.GetHiddenDataAt( iterator )->portrait;
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
void IO::OutputTurnOptions( )
{
	std::cout << "[W] Go up\n";
	std::cout << "[S] Go down\n";
	std::cout << "[A] Go left\n";
	std::cout << "[D] Go Right\n";
	std::cout << "[Q] Do nothing\n";
	std::cout << "[E] Exit to meny\n";
}
void IO::OutputGameState( GameState state )
{
	if( state == GameState::Won )
	{
		std::cout << "\nYou win!";
		std::cout << "\n\nPress enter to continue: ";
	}
	else if( state == GameState::Lost )
	{
		std::cout << "\nYou lose!";
		std::cout << "\n\nPress enter to continue: ";
	}
}

void IO::InputEnter( )
{
	/* First one doesn't wait for user input */
	std::cin.get( );
	std::cin.get( );
}
char IO::InputValidChar( const std::string& context, const std::vector<char>& valid )
{
	std::string choice;

	while( true )
	{
		IO::OutputString( context );
		std::cin >> choice;

		if( std::find( valid.begin( ), valid.end( ), choice[0] ) != valid.end( ) )
		{
			break;
		}
	}

	return choice[0];
}
int IO::InputPositiveInteger( const std::string& context )
{
	std::string choice;

	while( true )
	{
		IO::OutputString( context );
		std::cin >> choice;

		if( choice.size( ) < 10 &&
			std::all_of( choice.begin( ), choice.end( ), ::isdigit ) )
		{
			break;
		}
	}

	return std::stoi( choice );
}