#include "Functions.h"
#include "Enums.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <map>

double RandomNumberGenerator( double min, double max )
{
    static std::random_device rd;
    static std::mt19937 generator( rd( ) );
    std::uniform_real_distribution<double> randomNumber( min, max );

    return randomNumber( generator );
}
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
void PrintDungeonCentered( const Dungeon& dungeon, int visionReach, const Vector2<int>& center, const Vector2<int>& sizeScreen )
{
    const Vector2<int> origoCamera = center - sizeScreen / 2;
    const Vector2<int> iteratorBegin = origoCamera - 1;
    const Vector2<int> iteratorEnd   = origoCamera + 1 + sizeScreen;
    auto InsideVisionReach = [visionReach, center] ( const Vector2<int>& iterator ) -> bool
    {
        return
            iterator >= center - visionReach &&
            iterator <= center + visionReach;
    };
    Vector2<int> iterator;

    for( iterator.y = iteratorBegin.y; iterator.y <= iteratorEnd.y; iterator.y++ )
    {
        for( iterator.x = iteratorBegin.x; iterator.x <= iteratorEnd.x; iterator.x++ )
        {
            if( iterator.x == iteratorBegin.x ||
                iterator.y == iteratorBegin.y ||
                iterator.x == iteratorEnd.x ||
                iterator.y == iteratorEnd.y )
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

    if( combatant.healthRegen > 0 )
    {
        std::cout << "+";
    }

    std::cout << combatant.healthRegen << ")\n";
    std::cout << combatant.name << " spells owned: " << __popcnt( combatant.spells ) << "\n";
}
Vector2<int> PositionMove( const Vector2<int>& position, const Orientation& orientation )
{
    static const std::map<Orientation, Vector2<int>> directions =
    {
        { Orientation::North, {  0, -1 } },
        { Orientation::West,  { -1,  0 } },
        { Orientation::South, {  0,  1 } },
        { Orientation::East,  {  1,  0 } }
    };

    return position + directions.at( orientation );
}
Vector2<int> PositionMoveProbability( const Vector2<int>& position, int north, int west, int south, int east, int still )
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
Vector2<int> PositionRotateClockwise( const Vector2<int>& position, const Vector2<int>& sizeGrid )
{
    return { sizeGrid.x - position.y - 1, position.x };
}