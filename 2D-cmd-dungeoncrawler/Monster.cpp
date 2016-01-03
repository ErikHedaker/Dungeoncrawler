#include "Monster.h"
#include "Enums.h"

Monster::Monster( ) :
	Character( Vector2i( -1, -1 ), Portrait::Monster )
{ }
Monster::Monster( const Vector2i& position ) :
	Character( position, Portrait::Monster )
{ }