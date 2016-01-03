#include "Path.h"
#include "Enums.h"

Path::Path( ) :
	Entity( Vector2i( -1, -1 ), Portrait::Path )
{ }
Path::Path( const Vector2i& position ) :
	Entity( position, Portrait::Path )
{ }