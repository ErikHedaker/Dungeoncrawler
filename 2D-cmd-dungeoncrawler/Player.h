#pragma once

#include "Character.h"

class Player : public Character
{
	public:
		Player( const Vector2i& position );
		Player( const Vector2i& position, float speed, float armor, float damage, float health, float mana, int visionReach );
		
		int GetVisionReach( ) const;
		void SetVisionReach( int value );

	private:
		int _visionReach;
};