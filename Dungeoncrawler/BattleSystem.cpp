#include "BattleSystem.h"
#include "EntityFactory.h"
#include "Functions.h"
#include <iostream>
#include <utility>
#include <optional>
#include <cmath>
#include <string_view>
#include <array>

BattleSystem::BattleSystem( ) :
    _effects( []( std::vector<Effect> effects )
    {
        std::map<int, Effect> temp;

        for( int i = 0, limit = effects.size( ); i < limit; i++ )
        {
            temp.try_emplace( 1 << i, effects[i] );
        }

        return temp;
    }( LoadEffects( ) ) ),
    _spells( []( std::vector<Spell> spells )
    {
        std::map<int, Spell> temp;

        for( int i = 0, limit = spells.size( ); i < limit; i++ )
        {
            temp.try_emplace( 1 << i, spells[i] );
        }

        return temp;
    }( LoadSpells( ) ) )
{ }

void BattleSystem::Encounter( Character& player, Character& AI ) const
{
    std::string events;
    std::string output;
    bool flee = false;
    auto ExitCondition = [] ( const std::array<bool, 3> conditions )
    {
        static const std::array<std::string, 3> output
        {
            "lose!\n",
            "win!\n",
            "flee the battle!\n"
        };
        
        for( int i = 0, limit = conditions.size( ); i < limit; i++ )
        {
            if( conditions[i] )
            {
                std::cout << "\nYou " << output[i];
                std::cout << "Press enter to continue: ";
                InputEnter( );

                return true;
            }
        }

        return false;
    };

    while( true )
    {
        ClearScreen( );
        output.clear( );
        output
            .append( "> BATTLE <\n- " )
            .append( player.name )
            .append( " vs " )
            .append( AI.name )
            .append( "!\n\n> EVENTS <\n" )
            .append( events )
            .append( "\n> HEALTH <\n- " )
            .append( player.name )
            .append( ": " )
            .append( GetStringHealth( player.health ) )
            .append( "\n- " )
            .append( AI.name )
            .append( ": " )
            .append( GetStringHealth( AI.health ) )
            .append( "\n\n> ACTION <\n" );
        std::cout << output;

        if( ExitCondition( { !player.active, !AI.active, flee } ) )
        {
            break;
        }

        events.clear( );
        events += UpdateEffects( player );
        events += UpdateEffects( AI );
        events += TurnPlayer( player, AI, output, flee );
        events += TurnAI( AI, player );
        player.Update( );
        AI.Update( );
    }
}
std::string BattleSystem::TurnPlayer( Character& player, Character& enemy, std::string_view print, bool& flee ) const
{
    std::string events;
    char input;

    while( true )
    {
        std::string selection( print );

        ClearScreen( );
        selection
            .append( "[0] Flee\n" )
            .append( "[1] Attack with weapon\n" )
            .append( "[2] Attack with spell\n" );
        std::cout << selection;
        input = InputChar( "Select action: ", { '0', '1', '2' } );
        selection
            .append( "Select action: " )
            .append( std::string( 1, input ) )
            .append( "\n" );

        switch( input )
        {
            case '0':
            {
                events
                    .append( "- " )
                    .append( player.name )
                    .append( " attempt to flee!\n" );
                flee = true;

                return events;
            }
            case '1':
            {
                const int result = player.damage;

                events
                    .append( "- " )
                    .append( player.name )
                    .append( " attack and deal " )
                    .append( std::to_string( result ) )
                    .append( " damage to " )
                    .append( enemy.name )
                    .append( "!\n" );
                enemy.health.current -= result;

                return events;
            }
            case '2':
            {
                while( true )
                {
                    ClearScreen( );
                    std::cout << selection;

                    const std::optional<Spell> spell( InputSpell( GetSpells( player.spells ) ) );
                    const std::map<char, Character*> targets
                    {
                        { '0', nullptr },
                        { '1', &enemy },
                        { '2', &player }
                    };
                    Character* target;
                    char choice;

                    if( !spell )
                    {
                        break;
                    }

                    std::cout << "\n> DETAIL <\n";
                    std::cout << "- Power:   " << ( spell->power ? GetStringPower( *spell->power ) : std::string( "-" ) ) << "\n";
                    std::cout << "- Effects: " << GetStringEffects( GetEffects( spell->effects ) ) << "\n";
                    std::cout << "\n> TARGET <\n";
                    std::cout << "[0] Exit selection\n";
                    std::cout << "[1] " << enemy.name << "\n";
                    std::cout << "[2] " << player.name << "\n";
                    choice = InputChar( "Select target: ", { '0', '1', '2' } );
                    target = targets.at( choice );

                    if( target )
                    {
                        return CastSpell( player, *target, *spell );
                    }
                }

                break;
            }
        }
    }
}
std::string BattleSystem::TurnAI( Character& AI, Character& enemy ) const
{
    const int result = AI.damage;
    std::string events;

    events
        .append( "- " )
        .append( AI.name )
        .append( " attack and deal " )
        .append( std::to_string( result ) )
        .append( " damage to " )
        .append( enemy.name )
        .append( "!\n" );
    enemy.health.current -= result;

    return events;
}
std::string BattleSystem::UpdateEffects( Character& character ) const
{
    std::string events;

    for( auto it = character.effects.begin( ); it != character.effects.end( ); )
    {
        if( it->second.power )
        {
            const int result = GetPowerDiceRoll( *it->second.power );

            events
                .append( "- (" )
                .append( std::to_string( it->second.duration ) )
                .append( ")" )
                .append( it->second.name )
                .append( " ticks for " )
                .append( std::to_string( result ) )
                .append( " damage on " )
                .append( character.name )
                .append( "\n" );
            character.health.current -= result;
        }

        if( it->second.duration <= 0 )
        {
            events
                .append( "- " )
                .append( it->second.name )
                .append( " fades from " )
                .append( character.name )
                .append( "\n" );
            it = character.effects.erase( it );
        }
        else
        {
            it->second.duration--;
            it++;
        }
    }

    return events;
}
std::string BattleSystem::CastSpell( Character& caster, Character& target, const Spell& spell ) const
{
    std::string events;

    events
        .append( "- " )
        .append( caster.name )
        .append( " cast " )
        .append( spell.name );

    if( spell.power )
    {
        const int result = GetPowerDiceRoll( *spell.power );
        const int type = spell.power->beneficial ? -1 : 1;
        const std::string positive = std::string( "restore " ) + std::to_string( result ) + std::string( " health" );
        const std::string negative = std::string( "deal "    ) + std::to_string( result ) + std::string( " damage" );
        const std::string typeLine = spell.power->beneficial ? positive : negative;

        events
            .append( " and " )
            .append( typeLine )
            .append( " to " )
            .append( target.name )
            .append( "!\n" );
        target.health.current -= result * type;
    }
    else
    {
        events
            .append( " on " )
            .append( target.name )
            .append( "!\n" );
    }

    if( spell.effects != 0 )
    {
        const std::vector<Effect> effects = GetEffects( spell.effects );

        events
            .append( "- " )
            .append( spell.name )
            .append( " applies " )
            .append( GetStringEffects( effects ) )
            .append( " to " )
            .append( target.name )
            .append( "!\n" );

        for( const auto& effect : effects )
        {
            target.effects.insert_or_assign( effect.name, effect );
        }
    }

    return events;
}
std::optional<Spell> BattleSystem::InputSpell( const std::vector<Spell>& spells ) const
{
    std::vector<char> valid;
    char input;

    std::cout << "\n> SPELLS <\n";
    std::cout << "[0] Exit selection\n";
    valid.push_back( '0' );

    for( int i = 0, limit = spells.size( ); i < limit; i++ )
    {
        std::cout << "[" << i + 1 << "] " << spells[i].name << "\n";
        valid.push_back( i + '1' );
    }

    input = InputChar( "Select spell: ", valid );

    return input != '0' ? spells[input - '1'] : std::optional<Spell>( );
}
std::vector<Effect> BattleSystem::GetEffects( int bitmask ) const
{
    std::vector<Effect> temp;

    for( const auto& it : _effects )
    {
        if( it.first & bitmask )
        {
            temp.push_back( it.second );
        }
    }

    return temp;
}
std::vector<Spell> BattleSystem::GetSpells( int bitmask ) const
{
    std::vector<Spell> temp;

    for( const auto& it : _spells )
    {
        if( it.first & bitmask )
        {
            temp.push_back( it.second );
        }
    }

    return temp;
}