#include "BattleSystem.h"
#include "Functions.h"

void BattleSystem::EngageRandomMonster( Combatant& character )
{
	Combatant monster = GetRandomMonster( );
}
void BattleSystem::TempEngageRandomMonster( Combatant& character )
{
	Combatant monster = GetRandomMonster( );
	Combatant* attacker = &monster;
	Combatant* defender = &character;

	system( "CLS" );
	AsciiArtSwords( );

	while( true )
	{
		defender->health -= attacker->damage;

		if( defender->health <= 0 )
		{
			std::cout << '\n' << defender->name << " died.";

			break;
		}
		else
		{
			std::swap( attacker, defender );
		}
	}

	std::cout << "\n\nPress enter to continue: ";
	GetEnter( );
}
void BattleSystem::AddMonsterToLibrary( Combatant& monster )
{
	monsters.push_back( monster );
}
Combatant BattleSystem::GetRandomMonster( )
{
	return monsters[RandomNumberGenerator( 0, monsters.size( ) - 1 )];
}