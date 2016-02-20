#include "BattleSystem.h"
#include "Functions.h"
#include <algorithm>
#include <utility>
#include <iostream>

Spell::Spell( const std::string& name, double damageMultiplier ) :
	name( name ),
	damageMultiplier( damageMultiplier )
{ }

Combatant::Combatant( const std::string& name, int health, int healthMax, int healthRegeneration, int damage, int spells ) :
	name( name ),
	health( health ),
	healthMax( healthMax ),
	healthRegeneration( healthRegeneration ),
	damage( damage ),
	spells( spells )
{ }
void Combatant::Update( )
{
	health += healthRegeneration;

	if( health > healthMax )
	{
		health = healthMax;
	}
}

BattleSystem::BattleSystem( ) :
	_libraryMonsters
	( {
		{ "Rotten Zombie",  120, 120, 5, 20 },
		{ "Frozen Skeleton", 80,  80, 0, 10, Spells::Iceblast },
		{ "Burning Lunatic", 70,  70, 0, 10, Spells::Fireball }
	} ),
	_librarySpells
	( {
		{ Spells::TouchOfDeath, { "Touch of Death", 1000.0 } },
		{ Spells::Iceblast,     { "Iceblast",       1.2    } },
		{ Spells::Fireball,     { "Fireball",       1.5    } }
	} )
{ }

Combatant BattleSystem::GetRandomMonster( )
{
	return _libraryMonsters[RandomNumberGenerator( 0, _libraryMonsters.size( ) - 1 )];
}
Spell BattleSystem::GetSpell( Combatant& player )
{
	const int bitMax = _librarySpells.size( ) - 1;
	std::map<char, Spell> idSpell;
	std::vector<char> choices;
	char id = '1';
	char choice;

	std::cout << "\nChoose a spell:\n\n";

	for( int bit = 1; bit <= 1 << bitMax; bit <<= 1 )
	{
		if( player.spells & bit )
		{
			idSpell.emplace( id, _librarySpells.at( bit ) );
			choices.push_back( id );
			std::cout << "[" << id << "] " << idSpell.at( id ).name << "\n";
			id++;
		}
	}

	choice = GetValidChar( "\nEnter choice: ", choices );
	
	return idSpell.at( choice );
}
void BattleSystem::CastSpell( const Spell& spell, Combatant& caster, Combatant& target )
{
	const double random = RandomNumberGenerator( 90, 110 ) / 100.0;
	const int healthOld = target.health;

	target.health -= static_cast<int>( caster.damage * spell.damageMultiplier * random );

	std::cout << caster.name << " casts " << spell.name << " on " << target.name << ", which dealt " << healthOld - target.health << " damage!\n";
}
void BattleSystem::WeaponAttack( Combatant& attacker, Combatant& target )
{
	const double random = RandomNumberGenerator( 90, 110 ) / 100.0;
	const int healthOld = target.health;

	target.health -= static_cast<int>( attacker.damage * random );

	std::cout << attacker.name << " attacks " << target.name << " with a weapon, which dealt " << healthOld - target.health << " damage!\n";
}
void BattleSystem::PlayerTurn( Combatant& player, Combatant& monster )
{
	std::cout << "\n";
	std::cout << "[1] Attack with weapon\n";
	std::cout << "[2] Attack with spell\n\n";

	const std::vector<char> choices =
	{
		'1',
		'2'
	};
	const char choice = GetValidChar( "Enter choice: ", choices );


	switch( choice )
	{
		case '1':
		{
			system( "CLS" );
			WeaponAttack( player, monster );

			break;
		}
		case '2':
		{
			if( player.spells != 0 )
			{
				Spell spell = GetSpell( player );
				system( "CLS" );
				CastSpell( spell, player, monster );
			}

			break;
		}
	}
}
void BattleSystem::MonsterTurn( Combatant& player, Combatant& monster )
{
	const int bitMax = _librarySpells.size( ) - 1;
	const int actionsMax = __popcnt( monster.spells ) + 1;

	for( int bit = 1; bit <= 1 << bitMax; bit <<= 1 )
	{
		if( monster.spells & bit &&
			RandomNumberGenerator( 1, actionsMax ) == actionsMax )
		{
			CastSpell( _librarySpells.at( bit ), monster, player );

			return;
		}
	}

	WeaponAttack( monster, player );
}
void BattleSystem::EngageRandomMonster( Combatant& player )
{
	Combatant monster = GetRandomMonster( );

	system( "CLS" );
	std::cout << "You've been engaged in combat with a " << monster.name << "!\n";

	while( true )
	{
		std::cout << "\n-------------------\n\n";
		player.Update( );
		monster.Update( );
		PrintCombatantInformation( player );
		std::cout << "\n";
		PrintCombatantInformation( monster );
		PlayerTurn( player, monster );
		MonsterTurn( player, monster );

		if( player.health <= 0 ||
			monster.health <= 0 )
		{
			std::cout << "\n" << ( player.health <= 0 ? player.name : monster.name ) << " died!";
			std::cout << "\n\nPress enter to continue: ";
			GetEnter( );

			break;
		}
	}
}