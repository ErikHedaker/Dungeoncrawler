#include "Monster.h"
#include "Enums.h"

Monster::Monster( const Vector2i& position ) :
	Character( position, Portrait::Monster, 10.0f, 0.10f, 10.0f, 100.0f, 100.0f )
{ }