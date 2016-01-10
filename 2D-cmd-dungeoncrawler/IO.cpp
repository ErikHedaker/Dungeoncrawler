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
	std::cout << "[1] Build new dungeon (randomized)\n";
	std::cout << "[2] Build new dungeon (minor configuration)\n";
	std::cout << "[3] Load dungeon from file\n";
	std::cout << "[4] Exit\n";
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
					std::cout << dungeon.GetEntityDataAt( iterator )->portrait;
				}
				else
				{
					std::cout << Portrait::Ground;
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
					std::cout << dungeon.GetEntityDataAt( iterator )->portrait;
				}
				else
				{
					std::cout << Portrait::Ground;
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
	std::cout << "[E] Save and exit to meny\n";
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
void Output::AsciiArtSwords( )
{
	std::cout << "\t  n                                                                 :.  \n";
	std::cout << "\t  E%                                                                :'5 \n";
	std::cout << "\t z  %                                                              :' ` \n";
	std::cout << "\t K   ':                                                           z   R \n";
	std::cout << "\t ?     %.                                                       :^    J \n";
	std::cout << "\t  '.    ^s                                                     f     :~ \n";
	std::cout << "\t   '+.    #L                                                 z'    .*   \n";
	std::cout << "\t     '+     %L                                             z'    .~     \n";
	std::cout << "\t       ':    '%.                                         .#     +       \n";
	std::cout << "\t         ':    ^%.                                     .#`    +'        \n";
	std::cout << "\t           #:    'n                                  .+`   .z'          \n";
	std::cout << "\t             #:    ':                               z`    +'            \n";
	std::cout << "\t               %:   `*L                           z'    z'              \n";
	std::cout << "\t                 *:   ^*L                       z*   .+'                \n";
	std::cout << "\t                   's   ^*L                   z#   .*'                  \n";
	std::cout << "\t                     #s   ^%L               z#   .*'                    \n";
	std::cout << "\t                       #s   ^%L           z#   .r'                      \n";
	std::cout << "\t                         #s   ^%.       u#   .r'                        \n";
	std::cout << "\t                           #i   '%.   u#   .@'                          \n";
	std::cout << "\t                             #s   ^%u#   .@'                            \n";
	std::cout << "\t                               #s x#   .*'                              \n";
	std::cout << "\t                                x#`  .@%.                               \n";
	std::cout << "\t                              x#`  .d'  '%.                             \n";
	std::cout << "\t                            xf~  .r' #s   '%.                           \n";
	std::cout << "\t                      u   x*`  .r'     #s   '%.  x.                     \n";
	std::cout << "\t                      %Mu*`  x*'         #m.  '%zX'                     \n";
	std::cout << "\t                      :R(h x*              'h..*dN.                     \n";
	std::cout << "\t                    u@NM5e#>                 7?dMRMh.                   \n";
	std::cout << "\t                  z$@M@$#'#'                 *''*@MM$hL                 \n";
	std::cout << "\t                u@@MM8*                          '*$M@Mh.               \n";
	std::cout << "\t              z$RRM8F'                             'N8@M$bL             \n";
	std::cout << "\t             5`RM$#                                  'R88f)R            \n";
	std::cout << "\t             'h.$'                                     #$x*             \n";
}
void Output::BattleScreenStart( const Character& attacker, const Character& defender )
{
	Output::ClearScreen( );
	Output::AsciiArtSwords( );
	std::cout << "\n\n";
	std::cout << attacker.portrait << " has engaged " << defender.portrait;
}
void Output::BattleScreenEnd( const Character& winner, const Character& loser )
{
	std::cout << "\n";
	std::cout << loser.portrait << " has lost to " << winner.portrait;
	std::cout << "\n\nPress enter to continue: ";
	Input::Enter( );
}