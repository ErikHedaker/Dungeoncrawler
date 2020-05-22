#include "BattleSystem.h"
#include "EntityFactory.h"
#include "Functions.h"
#include <iostream>
#include <utility>
#include <cmath>
#include <string_view>
#include <array>
#include <memory>
#include <functional>

BattleSystem::BattleSystem( bool clear ) :
    _clearOutput( clear ),
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
                std::cout << "Press enter to continue: \n";
                SelectEnter( );

                return true;
            }
        }

        return false;
    };

    while( true )
    {
        ClearScreen( _clearOutput );
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

        ClearScreen( _clearOutput );
        selection
            .append( "[0] Flee\n" )
            .append( "[1] Attack with weapon\n" )
            .append( "[2] Attack with spell\n" )
            .append( "Select action: \n" );
        std::cout << selection;
        input = SelectChar( { '0', '1', '2' } );
        selection
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
                return AttackMelee( player, enemy );
            }
            case '2':
            {
                while( true )
                {
                    const Spell* spell;
                    Character* target;

                    ClearScreen( _clearOutput );
                    std::cout << selection;
                    SelectSpell( spell, GetSpells( player.spells ) );

                    if( !spell )
                    {
                        break;
                    }

                    DisplayDetail( *spell );
                    SelectTarget( target, { &enemy, &player } );

                    if( target )
                    {
                        return AttackSpell( player, *target, *spell );
                    }
                }

                break;
            }
        }
    }
}
std::string BattleSystem::TurnAI( Character& AI, Character& enemy ) const
{
    return AttackMelee( AI, enemy );
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
                .append( "- " )
                .append( it->second.name )
                .append( " affects " )
                .append( character.name )
                .append( ", damaging its health by " )
                .append( std::to_string( result ) )
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
std::string BattleSystem::DisplayDetail( const Spell& spell ) const
{
    std::string output;

    output
        .append( "\n> DETAIL <\n-   Power: " )
        .append( spell.power ? GetStringPower( *spell.power ) : std::string( "-" ) )
        .append( "\n- Effects: " )
        .append( spell.effects ? GetStringEffects( GetEffects( spell.effects ) ) : std::string( "-" ) )
        .append( "\n" );
    std::cout << output;

    return output;
}
std::string BattleSystem::SelectTarget( Character*& target, const std::vector<Character*>& targets ) const
{
    std::map<char, Character*> mapped;
    std::vector<char> valid;
    std::string output;
    char select;

    output
        .append( "\n> TARGET <\n" )
        .append( "[0] Exit selection\n" );
    mapped.emplace( '0', nullptr );
    valid.push_back( '0' );

    for( int i = 0, limit = targets.size( ); i < limit; i++ )
    {
        const char option = '1' + i;

        output
            .append( "[" )
            .append( std::string( 1, option ) )
            .append( "] " )
            .append( targets[i]->name )
            .append( "\n" );
        mapped.emplace( option, targets[i] );
        valid.push_back( option );
    }

    output.append( "Select target: \n" );
    std::cout << output;
    select = SelectChar( valid );
    output
        .append( std::string( 1, select ) )
        .append( "\n" );
    target = mapped.at( select );

    return output;
}
std::string BattleSystem::SelectSpell( const Spell*& spell, const std::vector<std::reference_wrapper<const Spell>>& spells ) const
{
    std::map<char, const Spell*> mapped;
    std::vector<char> valid;
    std::string output;
    char select;

    output
        .append( "\n> SPELLS <\n" )
        .append( "[0] Exit selection\n" );
    mapped.emplace( '0', nullptr );
    valid.push_back( '0' );

    for( int i = 0, limit = spells.size( ); i < limit; i++ )
    {
        const char option = '1' + i;

        output
            .append( "[" )
            .append( std::string( 1, option ) )
            .append( "] " )
            .append( spells[i].get( ).name )
            .append( "\n" );
        mapped.emplace( option, &spells[i].get( ) );
        valid.push_back( option );
    }

    output.append( "Select target: \n" );
    std::cout << output;
    select = SelectChar( valid );
    output
        .append( std::string( 1, select ) )
        .append( "\n" );
    spell = mapped.at( select );

    return output;
}
std::string BattleSystem::AttackSpell( Character& attacker, Character& target, const Spell& spell ) const
{
    std::string events;

    events
        .append( "- " )
        .append( attacker.name )
        .append( " cast " )
        .append( spell.name )
        .append( " on " )
        .append( target.name );

    if( spell.power )
    {
        const int result = GetPowerDiceRoll( *spell.power );
        const int type = spell.power->beneficial ? -1 : 1;

        events
            .append( ", " )
            .append( spell.power->beneficial ? "restoring" : "damaging" )
            .append( " its health by " )
            .append( std::to_string( result ) );
        target.health.current -= result * type;
    }

    events.append( "\n" );

    if( spell.effects != 0 )
    {
        const std::vector<std::reference_wrapper<const Effect>> effects = GetEffects( spell.effects );

        events
            .append( "- " )
            .append( spell.name )
            .append( " affects " )
            .append( target.name )
            .append( ", applying " )
            .append( GetStringEffects( effects ) )
            .append( "\n" );

        for( const auto& effect : effects )
        {
            target.effects.insert_or_assign( effect.get( ).name, effect.get( ) );
        }
    }

    return events;
}
std::string BattleSystem::AttackMelee( Character& attacker, Character& target ) const
{
    const int result = attacker.damage;
    std::string events;

    events
        .append( "- " )
        .append( attacker.name )
        .append( " attack " )
        .append( target.name )
        .append( ", damaging its health by " )
        .append( std::to_string( result ) )
        .append( "\n" );
    target.health.current -= result;

    return events;
}
std::vector<std::reference_wrapper<const Effect>> BattleSystem::GetEffects( int bitmask ) const
{
    std::vector<std::reference_wrapper<const Effect>> temp;

    for( const auto& it : _effects )
    {
        if( it.first & bitmask )
        {
            temp.push_back( std::cref( it.second ) );
        }
    }

    return temp;
}
std::vector<std::reference_wrapper<const Spell>> BattleSystem::GetSpells( int bitmask ) const
{
    std::vector<std::reference_wrapper<const Spell>> temp;

    for( const auto& it : _spells )
    {
        if( it.first & bitmask )
        {
            temp.push_back( std::cref( it.second ) );
        }
    }

    return temp;
}