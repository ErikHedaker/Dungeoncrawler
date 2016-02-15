#pragma once

#include "Player.h"
#include "Combatant.h"
#include <vector>

class BattleSystem
{
	public:
		void EngageRandomMonster( Combatant& character );
		void TempEngageRandomMonster( Combatant& character );
		void AddMonsterToLibrary( Combatant& monster );
		Combatant GetRandomMonster( );

	private:
		std::vector<Combatant> monsters;
};