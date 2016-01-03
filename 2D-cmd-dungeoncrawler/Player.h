#pragma once

#include "Character.h"

class Player : public Character
{
	public:
		Player( );
		Player( const Vector2i& position );
		Player( const Vector2i& position, float speed, float armor, float damage, float health, float mana );
};