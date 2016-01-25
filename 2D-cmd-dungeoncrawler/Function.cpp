#include "Functions.h"
#include "Enums.h"
#include "Dungeon.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <random>

int RandomNumberGenerator( int min, int max )
{
	static std::random_device rd;
	static std::mt19937 generator( rd( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}

void OutputClearScreen( )
{
	/* Some day I will switch to a framework like ncurses and get rid of this abomination */
	system( "CLS" );
}
void OutputDungeonCentered( const Dungeon& dungeon, const Vector2i& center )
{
	const Vector2i screenSize = { 40, 20 };
	const Vector2i cameraOrigo = center - screenSize / 2;
	const Vector2i iteratorBegin = cameraOrigo - 1;
	const Vector2i iteratorEnd = cameraOrigo + screenSize + 1;
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
					 dungeon.GetVision( iterator ) )
			{
				std::cout << dungeon.GetTile( iterator ).icon;
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
void OutputDungeonFull( const Dungeon& dungeon )
{
	const int maxCol = dungeon.GetSize( ).first;
	const int maxRow = dungeon.GetSize( ).second;
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < maxCol; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < maxRow; iterator.col++ )
		{
			if( dungeon.GetVision( iterator ) )
			{
				std::cout << dungeon.GetTile( iterator ).icon;
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
void OutputCharacterStatus( const Character& character )
{
	std::cout << "Health: " << character.GetHealth( ) << "\n";
	std::cout << "Mana:   " << character.GetMana( ) << "\n";
	std::cout << "\n";

}
void OutputTurnOptions( )
{
	std::cout << "[W] Go North\n";
	std::cout << "[A] Go East\n";
	std::cout << "[S] Go South\n";
	std::cout << "[D] Go West\n";
	std::cout << "[Q] Stand still\n";
	std::cout << "[E] Exit to meny\n";
	std::cout << "[F] Rotate dungeon 90 degrees clockwise\n";
}
void OutputAsciiArtSpider( )
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
void OutputAsciiArtSwords( )
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
void OutputBattleScreenStart( const Character& attacker, const Character& defender )
{
	OutputClearScreen( );
	OutputAsciiArtSwords( );
	std::cout << "\n\n";
	std::cout << "Someone" << " has engaged " << "Someone else";
}
void OutputBattleScreenEnd( const Character& winner, const Character& loser )
{
	std::cout << "\n";
	std::cout << "Someone" << " has lost to " << "Someone else";
	std::cout << "\n\nPress enter to continue: ";
	InputEnter( );
}

void InputEnter( )
{
	/* First one doesn't wait for user input */
	std::cin.get( );
	std::cin.get( );
}
char InputValidChar( const std::string& context, const std::vector<char>& valid )
{
	std::string choice;

	while( true )
	{
		std::cout << context;
		std::cin >> choice;

		if( std::find( valid.begin( ), valid.end( ), choice[0] ) != valid.end( ) )
		{
			break;
		}
	}

	return choice[0];
}
int InputPositiveInteger( const std::string& context )
{
	std::string choice;

	while( true )
	{
		std::cout << context;
		std::cin >> choice;

		if( choice.size( ) < 10 &&
			std::all_of( choice.begin( ), choice.end( ), ::isdigit ) )
		{
			break;
		}
	}

	return std::stoi( choice );
}