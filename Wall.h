#pragma once

#include "Entity.h"

class Wall : public Entity
{
	public:
		Wall( const Vector2i& position, char portrait );
};