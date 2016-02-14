#pragma once

#include "Vector2i.h"
#include "Enums.h"

class Player
{
	public:
		Player( );

		Vector2i position;
		int visionReach;
		PlayerStatus status;

};