#include "BattleSystem.h"
#include "EntityFactory.h"
#include "Functions.h"
#include <iostream>
#include <utility>
#include <optional>
#include <cmath>
#include <string_view>

BattleSystem::BattleSystem( ) :
    _effects( []( const std::vector<Effect>& effects )
    {
        std::map<int, Effect> temp;

        for( int i = 0, limit = effects.size( ); i < limit; i++ )
        {
            temp.emplace( std::make_pair( 1 << i, effects[i] ) );
        }

        return temp;
    }( LoadEffects( ) ) ),
    _spells( []( const std::vector<Spell>& spells )
    {
        std::map<int, Spell> temp;

        for( int i = 0, limit = spells.size( ); i < limit; i++ )
        {
            temp.emplace( std::make_pair( 1 << i, spells[i] ) );
        }

        return temp;
    }( LoadSpells( ) ) )
{ }

void BattleSystem::Encounter( Character& player, Character& AI ) const
{
    std::string events;
    std::string print;
    bool flee = false;

    while( true )
    {
        ClearScreen( );
        print.clear( );
        print += "> BATTLE <\n";
        print += "- ";
        print += player.name;
        print += " vs ";
        print += AI.name;
        print += "!\n";
        print += "\n";
        print += "> EVENTS <\n";
        print += events;
        print += "\n";
        print += "> HEALTH <\n";
        print += "- ";
        print += GetHealth( player );
        print += "- ";
        print += GetHealth( AI );
        print += "\n";
        print += "> PLAYER <\n";
        std::cout << print;
        events.clear( );

        if( !AI.active )
        {
            std::cout << "\nYou win!\n";
            std::cout << "Press enter to continue: ";
            InputEnter( );
            break;
        }
        if( !player.active )
        {
            std::cout << "\nYou lose!\n";
            std::cout << "Press enter to continue: ";
            InputEnter( );
            break;
        }
        if( flee )
        {
            std::cout << "\nYou flee the battle!\n";
            std::cout << "Press enter to continue: ";
            InputEnter( );
            break;
        }

        TurnPlayer( player, AI, events, print, flee );
        TurnAI( AI, player, events );
        Update( player, events );
        Update( AI, events );
    }
}
void BattleSystem::TurnPlayer( Character& player, Character& enemy, std::string& events, std::string_view print, bool& flee ) const
{
    char input;

    while( true )
    {
        ClearScreen( );
        std::cout << print;
        std::cout << "[0] Flee\n";
        std::cout << "[1] Attack with weapon\n";
        std::cout << "[2] Attack with spell\n";
        input = InputChar( "Enter choice: ", { '0', '1', '2' } );

        switch( input )
        {
            case '0':
            {
                flee = true;

                events += "- ";
                events += player.name;
                events += " attempt to flee!\n";

                return;
            }
            case '1':
            {
                const int result = player.damage < 0 ? player.damage : player.damage * ( -1 );

                events += "- ";
                events += player.name;
                events += " attack and deal ";
                events += std::to_string( std::abs( result ) );
                events += " damage to ";
                events += enemy.name;
                events += "!\n";
                enemy.health.current += result;

                return;
            }
            case '2':
            {
                const std::optional<Spell> spell = InputSpell( GetSpells( player.spells ) );

                if( !spell )
                {
                    ClearScreen( );
                    break;
                }

                if( spell->power )
                {
                    const int result = GetPowerDiceRoll( *spell->power );
                    const std::string positive = std::string( "restore " ) + std::to_string( result ) + std::string( " health" );
                    const std::string negative = std::string( "deal " ) + std::to_string( std::abs( result ) ) + std::string( " damage" );
                    const std::string type = spell->power->beneficial ? positive : negative;

                    events += "- ";
                    events += player.name;
                    events += " cast ";
                    events += spell->name;
                    events += " and ";
                    events += type;
                    events += " to ";
                    events += enemy.name;
                    events += "!\n";
                    enemy.health.current += result;
                }

                if( spell->effects != 0 )
                {
                    const std::vector<Effect> effects = GetEffects( spell->effects );
                    bool initial = false;

                    events += "- ";
                    events += spell->name;
                    events += " applies ";

                    for( int i = 0, limit = effects.size( ); i < limit; i++ )
                    {
                        events += effects[i].name;
                        enemy.effects.insert_or_assign( effects[i].name, effects[i] );

                        if( i == limit - 1 )
                        {
                            events += " to ";
                            events += enemy.name;
                            events += "!\n";

                        }
                        else if( i == limit - 2 )
                        {
                            events += " and ";
                        }
                        else
                        {
                            events += ", ";
                        }
                    }
                }

                return;
            }
        }
    }
}
void BattleSystem::TurnAI( Character& AI, Character& enemy, std::string& events ) const
{
    const int result = AI.damage < 0 ? AI.damage : AI.damage * ( -1 );

    events += "- ";
    events += AI.name;
    events += " attack and deal ";
    events += std::to_string( std::abs( result ) );
    events += " damage to ";
    events += enemy.name;
    events += "!\n";
    enemy.health.current += result;

    return;
}
void BattleSystem::Update( Character& character, std::string& events ) const
{
    for( auto it = character.effects.begin( ); it != character.effects.end( ); )
    {
        if( it->second.power )
        {
            const int result = GetPowerDiceRoll( *it->second.power );

            events += "- ";
            events += it->second.name;
            events += "(";
            events += std::to_string( it->second.duration );
            events += ") ticks for ";
            events += std::to_string( std::abs( result ) );
            events += " damage on ";
            events += character.name;
            events += "!\n";
            character.health.current += result;
        }

        if( it->second.duration <= 0 )
        {
            events += "- ";
            events += it->second.name;
            events += " fades from ";
            events += character.name;
            events += "!\n";
            it = character.effects.erase( it );
        }
        else
        {
            it->second.duration--;
            it++;
        }
    }

    character.Update( );
}
std::optional<Spell> BattleSystem::InputSpell( const std::vector<Spell>& spells ) const
{
    std::vector<char> valid;
    char input;

    std::cout << "\n> SPELLS <\n";
    std::cout << "[0] Exit spell selection\n";
    valid.push_back( '0' );

    for( int i = 0, limit = spells.size( ); i < limit; i++ )
    {
        std::cout << "[" << i + 1 << "] Cast " << spells[i].name << "\n";
        valid.push_back( i + '1' );
    }

    input = InputChar( "Enter choice: ", valid );

    return input != '0' ? spells[input - '1'] : std::optional<Spell>( );
}
std::vector<Effect> BattleSystem::GetEffects( int bitmask ) const
{
    std::vector<Effect> effects;

    for( const auto& it : _effects )
    {
        if( it.first & bitmask )
        {
            effects.push_back( it.second );
        }
    }

    return effects;
}
std::vector<Spell> BattleSystem::GetSpells( int bitmask ) const
{
    std::vector<Spell> spells;

    for( const auto& it : _spells )
    {
        if( it.first & bitmask )
        {
            spells.push_back( it.second );
        }
    }

    return spells;
}