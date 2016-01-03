#include "Wall.h"
#include "Enums.h"

Wall::Wall( ) :
	Entity( Vector2i( -1, -1 ), Portrait::Wall )
{ }
Wall::Wall( const Vector2i& position ) :
	Entity( position, Portrait::Wall )
{ }