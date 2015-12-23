#pragma once

#include "Entity.h"
#include "Orientation.h"

class Character : public Entity
{
	public:
		Character( const Vector2i& position, char portrait );

		void Move( Orientation orientation );
		void MoveTowards( const Vector2i& position );
		void MoveProbability( int north, int east, int south, int west, int still );
		void RevertPosition( );

	private:
		Vector2i _positionPrev;
};