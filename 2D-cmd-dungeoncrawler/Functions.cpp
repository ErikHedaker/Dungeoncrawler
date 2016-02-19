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
void PrintCombatantInformation( const Combatant& combatant )
{
	std::cout << combatant.name << " HP: " << combatant.health << " (";

	if( combatant.healthRegeneration > 0 )
	{
		std::cout << "+";
	}

	std::cout << combatant.healthRegeneration << ")\n";
	std::cout << combatant.name << " spells owned: " << __popcnt( combatant.spells ) << "\n";
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
	const int random = RandomNumberGenerator( 0, north + west + south + east + still - 1 );

	if( random < north )
	{
		return PositionMove( position, Orientation::North );
	}

	if( random < north + west )
	{
		return PositionMove( position, Orientation::West );
	}

	if( random < north + west + south )
	{
		return PositionMove( position, Orientation::South );
	}

	if( random < north + west + south + east )
	{
		return PositionMove( position, Orientation::East );
	}

	return position;
}