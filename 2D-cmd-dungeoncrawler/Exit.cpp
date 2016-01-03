#include "Exit.h"
#include "Enums.h"

Exit::Exit( ) :
	Entity( Vector2i( -1, -1 ), Portrait::Exit )
{ }
Exit::Exit( const Vector2i& position ) :
	Entity( position, Portrait::Exit )
{ }