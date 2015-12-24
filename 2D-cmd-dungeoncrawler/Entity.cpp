#include "Entity.h"

Entity::Entity( const Vector2i& position, char portrait ) :
	_position( position ),
	portrait( portrait )
{ }

void Entity::SetPosition( const Vector2i& position )
{
	_position.col = position.col;
	_position.row = position.row;
}
const Vector2i Entity::GetPosition( ) const
{
	return _position;
}