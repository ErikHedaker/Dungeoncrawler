#pragma once

#include "Character.h"

class Player : public Character
{
	public:
		Player( );
		Player( const Vector2i& position );
		Player( const Vector2i& position, float speed, float armor, float damage, float health, float mana, int lineOfSight );
		
		int GetLineOfSight( ) const;
		void SetLineOfSight( int value );

	private:
		int _lineOfSight;
};