#pragma once

#include <string>

struct Combatant
{
	Combatant( const std::string& name, int health, int damage, int abilities );

	const std::string name;
	int health;
	int damage;
	int abilities;
};