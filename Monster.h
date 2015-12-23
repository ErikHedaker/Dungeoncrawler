#include "Character.h"
#include <array>

class Monster : public Character
{
	public:
		Monster( const Vector2i& position, char portrait );
};