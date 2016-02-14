#include "Functions.h"
#include "Vector2i.h"
#include "Enums.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <map>

int RandomNumberGenerator( int min, int max )
{
	static std::random_device rd;
	static std::mt19937 generator( rd( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}
int GetPositiveInteger( const std::string& context )
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
char GetValidChar( const std::string& context, const std::vector<char>& valid )
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
void GetEnter( )
{
	/* First one doesn't wait for user input */
	std::cin.get( );
	std::cin.get( );
}
void AsciiArtSpider( )
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
void AsciiArtSwords( )
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
void PrintDungeonCentered( const Dungeon& dungeon, int visionReach, const Vector2i& center, const Vector2i& screenSize )
{
	const Vector2i cameraOrigo = center - screenSize / 2;
	const Vector2i iteratorBegin = cameraOrigo - 1;
	const Vector2i iteratorEnd = cameraOrigo + screenSize + 1;
	auto InsideVisionReach = [visionReach, center] ( const Vector2i& iterator ) -> bool
	{
		return
			iterator >= center - visionReach &&
			iterator <= center + visionReach;
	};
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
				std::cout << '\\';
			}
			else if( dungeon.InBounds( iterator ) &&
					 InsideVisionReach( iterator ) )
			{
				std::cout << dungeon.GetTile( iterator ).icon;
			}
			else if( dungeon.InBounds( iterator ) &&
					 dungeon.GetVision( iterator ) )
			{
				std::cout << ':';
			}
			else
			{
				std::cout << ' ';
			}
		}

		std::cout << '\n';
	}

	std::cout << '\n';
}
void Battle( int* attackerHealth, int* attackerDamage, int* defenderHealth, int* defenderDamage )
{
	system( "CLS" );
	AsciiArtSwords( );

	while( true )
	{
		*defenderHealth -= *attackerDamage;

		if( *defenderHealth <= 0 )
		{
			break;
		}
		else
		{
			std::swap( attackerHealth, defenderHealth );
			std::swap( attackerDamage, defenderDamage );
		}
	}

	std::cout << "\nSomeone died.";
	GetEnter( );
}
Vector2i PositionRotateClockwise( const Vector2i& position, int maxCol )
{
	return { maxCol - position.row - 1, position.col };
}
Vector2i PositionMove( const Vector2i& position, const Orientation& orientation )
{
	static const std::map<Orientation, Vector2i> directions =
	{
		{ Orientation::North, {  0, -1 } },
		{ Orientation::West,  { -1,  0 } },
		{ Orientation::South, {  0,  1 } },
		{ Orientation::East,  {  1,  0 } }
	};

	return position + directions.at( orientation );
}
Vector2i PositionMoveProbability( const Vector2i& position, int north, int west, int south, int east, int still )
{
	const int random = RandomNumberGenerator( 1, north + west + south + east + still );

	if( random <= north )
	{
		return PositionMove( position, Orientation::North );
	}
	
	if( random <= north + west )
	{
		return PositionMove( position, Orientation::West );
	}
	
	if( random <= north + west + south )
	{
		return PositionMove( position, Orientation::South );
	}
	
	if( random <= north + west + south + east )
	{
		return PositionMove( position, Orientation::East );
	}

	return position;
}