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
    std::uniform_real_distribution<double> random( min, max );

    return random( generator );
}
int RandomNumberGenerator( int min, int max )
{
    static std::random_device rd;
    static std::mt19937 generator( rd( ) );
    std::uniform_int_distribution<int> random( min, max );

    return random( generator );
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

        if( std::find( valid.begin( ), valid.end( ), choice.back( ) ) != valid.end( ) )
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
void PrintHealth( const Character& combatant )
{
    std::cout << combatant.name << " HP: " << combatant.health << " (";

    if( combatant.healthRegen > 0 )
    {
        std::cout << "+";
    }

    std::cout << combatant.healthRegen << ")\n";
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
Ability GetAbility( const std::vector<Ability>& abilities )
{
    std::map<char, Ability> abilitiesMap;
    std::vector<char> choices;
    char id = '1';
    
    std::cout << "\nChoose a spell:\n\n";

    for( const auto& ability : abilities )
    {
        abilitiesMap.emplace( id, ability );
        choices.push_back( id );
        std::cout << "[" << id << "] " << abilitiesMap.at( id ).name << "\n";
        id++;
    }
    
    return abilitiesMap.at( GetValidChar( "Enter choice: ", choices ) );
}
std::string UseAbility( const Ability& ability, Character& caster, Character& target )
{
    const int healthOld = target.health;

    target.health -= static_cast<int>( ability.damage * RandomNumberGenerator( 0.9, 1.1 ) );

    return std::string( caster.name + " casts " + ability.name + " on " + target.name + ", which dealt " + std::to_string( healthOld - target.health ) + " damage!\n" );
}
std::string UseWeapon( Character& attacker, Character& target )
{
    const int healthOld = target.health;

    target.health -= static_cast<int>( attacker.damage * RandomNumberGenerator( 0.9, 1.1 ) );

    return std::string( attacker.name + " attacks " + target.name + " with a weapon, which dealt " + std::to_string( healthOld - target.health ) + " damage!\n" );
}
std::string TurnPlayer( Character& player, Character& AI )
{
    const std::vector<char> choices = { '1', '2' };
    char choice;
    bool done = false;
    std::string result;

    while( !done )
    {
        std::cout << "\n";
        std::cout << "[1] Attack with weapon\n";
        std::cout << "[2] Attack with spell\n\n";
        choice = GetValidChar( "Enter choice: ", choices );

        switch( choice )
        {
            case '1':
            {
                result = UseWeapon( player, AI );
                done = true;

                break;
            }
            case '2':
            {
                if( player.abilities.size( ) > 0 )
                {
                    result = UseAbility( GetAbility( player.abilities ), player, AI );
                    done = true;
                }

                break;
            }
        }
    }

    return result;
}
std::string TurnAI( Character& player, Character& AI )
{
    const int number = RandomNumberGenerator( 0, AI.abilities.size( ) );

    return ( number == AI.abilities.size( ) ? UseWeapon( AI, player ) : UseAbility( AI.abilities[number], AI, player ) );
}
void Combat( Character& player, Character& AI )
{
    std::string previousTurnPlayer;
    std::string previousTurnAI;

    while( true )
    {
        system( "CLS" );
        std::cout << "You've been engaged in combat with a " << AI.name << "!\n\n";
        std::cout << "-----\n\n";

        player.Update( );
        AI.Update( );

        PrintHealth( player );
        PrintHealth( AI );
        
        std::cout << previousTurnPlayer << "\n\n";
        std::cout << previousTurnAI << "\n\n";
        std::cout << "-----\n\n";

        previousTurnPlayer = TurnPlayer( player, AI );
        previousTurnAI = TurnAI( player, AI );

        if( player.health <= 0 ||
            AI.health <= 0 )
        {
            std::cout << "\n-----\n\n";
            std::cout << ( player.health <= 0 ? player.name : AI.name ) << " died!";
            std::cout << "\n\nPress enter to continue: ";
            GetEnter( );

            break;
        }
    }
}