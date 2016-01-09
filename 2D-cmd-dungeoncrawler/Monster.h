#pragma once

#include "Character.h"
#include <array>

class Monster : public Character
{
	public:
		Monster( const Vector2i& position );
		Monster( const Vector2i& position, float speed, float armor, float damage, float health, float mana );
};