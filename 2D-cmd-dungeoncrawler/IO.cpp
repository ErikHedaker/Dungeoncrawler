#include "IO.h"
#include "Enums.h"
#include <iostream>
#include <string>
#include <algorithm>

extern const Vector2i WINDOW_SIZE;

void Input::Enter( )
{
	/* First one doesn't wait for user input */
	std::cin.get( );
	std::cin.get( );
}
char Input::ValidChar( const std::string& context, const std::vector<char>& valid )
{
	std::string choice;

	while( true )
	{
		Output::String( context );
		std::cin >> choice;

		if( std::find( valid.begin( ), valid.end( ), choice[0] ) != valid.end( ) )
		{
			break;
		}
	}

	return choice[0];
}
int Input::PositiveInteger( const std::string& context )
{
	std::string choice;

	while( true )
	{
		Output::String( context );
		std::cin >> choice;

		if( choice.size( ) < 10 &&
			std::all_of( choice.begin( ), choice.end( ), ::isdigit ) )
		{
			break;
		}
	}

	return std::stoi( choice );
}

void Output::ClearScreen( )
{
	/* Some day I will switch to a framework like ncurses and get rid of this abomination */
	system( "CLS" );
}
void Output::GameTypes( )
{
	std::cout << "Choose the game type.\n\n";
	std::cout << "[1] Randomized\n";
	std::cout << "[2] Configurated\n";
	std::cout << "[3] Exit game\n";
}
void Output::String( const std::string& string )
{
	std::cout << string;
}
void Output::DungeonCentered( const Dungeon& dungeon, const Vector2i& dungeonSize, const Vector2i& positionCenter )
{
	const Vector2i screenRadius = WINDOW_SIZE / 5;	// Drawn screen is not 1:1 ratio to the window screen.
	const Vector2i cameraOrigo = positionCenter - screenRadius;
	const Vector2i iteratorBegin = cameraOrigo - 1;
	const Vector2i iteratorEnd = cameraOrigo + screenRadius * 2 + 1;
	Vector2i iterator;

	for( iterator.row = iteratorBegin.row; iterator.row <= iteratorEnd.row; iterator.row++ )
	{
		for( iterator.col = iteratorBegin.col; iterator.col <= iteratorEnd.col; iterator.col++ )
		{
			if( iterator.col == iteratorBegin.col ||
				iterator.row == iteratorBegin.row ||
				iterator.col == iteratorEnd.col ||
				iterator.row == iteratorEnd.row )
			{
				std::cout << "\\";
			}
			else if( dungeon.InBounds( iterator ) &&
					 dungeon.GetVisionDataAt( iterator ) == true )
			{
				if( dungeon.GetEntityDataAt( iterator ) != nullptr )
				{
					std::cout << static_cast<std::underlying_type<Portrait>::type>( dungeon.GetEntityDataAt( iterator )->portrait );
				}
				else
				{
					std::cout << static_cast<std::underlying_type<Portrait>::type>( Portrait::Floor );
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
void Output::DungeonFull( const Dungeon& dungeon, const Vector2i& dungeonSize )
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
					std::cout << static_cast<std::underlying_type<Portrait>::type>( dungeon.GetEntityDataAt( iterator )->portrait );
				}
				else
				{
					std::cout << static_cast<std::underlying_type<Portrait>::type>( Portrait::Floor );
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
void Output::DungeonFullHidden( const Dungeon& dungeon, const Vector2i& dungeonSize )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < dungeonSize.col; iterator.col++ )
		{
			if( dungeon.GetHiddenDataAt( iterator ) != nullptr )
			{
				std::cout << static_cast<std::underlying_type<Portrait>::type>( dungeon.GetHiddenDataAt( iterator )->portrait );
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
void Output::PlayerStatus( const Player& player )
{
	std::cout << "Health: " << player.GetHealth( ) << "\n";
	std::cout << "Mana: " << player.GetMana( ) << "\n";
	std::cout << "\n";

}
void Output::TurnOptions( )
{
	std::cout << "[W] Go up\n";
	std::cout << "[S] Go down\n";
	std::cout << "[A] Go left\n";
	std::cout << "[D] Go Right\n";
	std::cout << "[Q] Do nothing\n";
	std::cout << "[E] Exit to meny\n";
}
void Output::GameStatusEnd( const GameStatus& status )
{
	if( status == GameStatus::Won )
	{
		std::cout << "\nYou win!";
		std::cout << "\n\nPress enter to continue: ";
	}
	else if( status == GameStatus::Lost )
	{
		std::cout << "\nYou lose!";
		std::cout << "\n\nPress enter to continue: ";
	}
}
void Output::AsciiArtSpider( )
{
	std::cout << "\n	  ;               ,";
	std::cout << "\n        ,;                 '.";
	std::cout << "\n       ;:                   :;";
	std::cout << "\n       ::                    :;";
	std::cout << "\n      ::                     ::";
	std::cout << "\n      ':                     ,:";
	std::cout << "\n      :.                    ,:";
	std::cout << "\n   ;' ::                    ::  ,";
	std::cout << "\n  .'  ';                   ;'   '.";
	std::cout << "\n  ::    :;                 ;:    ::";
	std::cout << "\n   ;      :;.             ,;:     ::";
	std::cout << "\n   :;      :;:           ,;\"      ::\"";
	std::cout << "\n   ::.      ':;  ;.,.;  ;:'    ,.;:";
	std::cout << "\n    \"'\"...   '::,:::::;:;.; \"\"'";
	std::cout << "\n       '\"\"\"....;:::::::;,,;.; \"\"\"";
	std::cout << "\n   .:::.....'\"':::::::::;\",..;::::;.";
	std::cout << "\n ;:' '\"\"'\"\";.,;:::::::::;.'\"\"\"\"\"\"  ';";
	std::cout << "\n ::'         ;::;:::;:::;.         ':;";
	std::cout << "\n::         ,;:::::::::::;:..        ::";
	std::cout << "\n;'     ,;;:;::::::::::::::;\":;..    ':.";
	std::cout << "\n::     ;:\"  ::::::\"\"\"'::::::   \":   ::";
	std::cout << "\n :.    ::   :::::;   '::::::  ::   ';";
	std::cout << "\n  ;    ::   ::::::;  :::::::  ;:   ;";
	std::cout << "\n  :    ::   :::::\"'  '':::   :;'   '";
	std::cout << "\n  '   ::    :::::::::::':\"   ::";
	std::cout << "\n      ::     ':::::::::\"'    ::";
	std::cout << "\n      ':       \"\"\"\"\"\"\"'      ::";
	std::cout << "\n       ::                   ;:";
	std::cout << "\n      ':;                 ;:\"";
	std::cout << "\n	';              ,;'";
	std::cout << "\n          \"'           '";
}
void Output::BattleScreenStart( const Character& attacker, const Character& defender )
{
	char attackerPortrait = static_cast<std::underlying_type<Portrait>::type>( attacker.portrait );
	char defenderPortrait = static_cast<std::underlying_type<Portrait>::type>( defender.portrait );

	Output::ClearScreen( );
	Output::AsciiArtSpider( );
	std::cout << "\n\n";
	std::cout << attackerPortrait << " has engaged " << defenderPortrait;
}
void Output::BattleScreenEnd( const Character& winner, const Character& loser )
{
	char winnerPortrait = static_cast<std::underlying_type<Portrait>::type>( winner.portrait );
	char loserPortrait = static_cast<std::underlying_type<Portrait>::type>( loser.portrait );

	std::cout << "\n";
	std::cout << loserPortrait << " has lost to " << winnerPortrait;
	std::cout << "\n\nPress enter to continue: ";
	Input::Enter( );
}