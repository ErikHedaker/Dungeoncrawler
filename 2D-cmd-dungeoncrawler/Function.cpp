#include "Functions.h"
#include "Enums.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <map>

void GetEnter( )
{
	/* First one doesn't wait for user input */
	std::cin.get( );
	std::cin.get( );
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

int RandomNumberGenerator( int min, int max )
{
	static std::random_device rd;
	static std::mt19937 generator( rd( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}

Vector2i RotatePositionClockwise( const Vector2i& position, int maxCol )
{
	Vector2i rotated;

	rotated.col = maxCol - position.row - 1;
	rotated.row = position.col;

	return rotated;
}
Vector2i MoveEntity( const Vector2i& position, const Orientation& orientation )
{
	static const std::map<Orientation, Vector2i> directions =
	{
		{ Orientation::North, Vector2i(  0, -1 ) },
		{ Orientation::West,  Vector2i( -1,  0 ) },
		{ Orientation::South, Vector2i(  0,  1 ) },
		{ Orientation::East,  Vector2i(  1,  0 ) }
	};

	return position + directions.at( orientation );
}
Vector2i MoveEntityProbability( const Vector2i& position, int north, int south, int west, int east, int still )
{
	const int sumProbability = north + south + west + east + still;
	const int random = RandomNumberGenerator( 0, sumProbability - 1 );
	Vector2i result = position;

	if( random < north )
	{
		result = MoveEntity( position, Orientation::North );
	}
	else if( random < north + south )
	{
		result = MoveEntity( position, Orientation::South );
	}
	else if( random < north + south + west )
	{
		result = MoveEntity( position, Orientation::West );
	}
	else if( random < north + south + west + east )
	{
		result = MoveEntity( position, Orientation::East );
	}

	return result;
}
void Battle( int* attackerHealth, int* attackerDamage, int* defenderHealth, int* defenderDamage )
{
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