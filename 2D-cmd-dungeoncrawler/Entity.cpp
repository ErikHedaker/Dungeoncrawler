#include "Entity.h"

Entity::Entity( const Vector2i& position, char portrait ) :
	portrait( portrait ),
	_position( position )
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