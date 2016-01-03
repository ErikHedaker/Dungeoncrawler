#pragma once

#include "Entity.h"

class Wall : public Entity
{
	public:
		Wall( );
		Wall( const Vector2i& position );
};