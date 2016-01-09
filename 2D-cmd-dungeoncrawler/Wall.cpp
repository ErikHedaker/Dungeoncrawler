#include "Wall.h"
#include "Enums.h"

Wall::Wall( const Vector2i& position ) :
	Entity( position, Portrait::Wall )
{ }