#include "Entity.h"

Entity::Entity( )
{
	_position.col = 0;
	_position.row = 0;
}
Entity::Entity( const Vector2i& newPosition )
{
	_position.col = newPosition.col;
	_position.row = newPosition.row;
}

void Entity::SetPosition( const Vector2i& newPosition )
{
	_position.col = newPosition.col;
	_position.row = newPosition.row;
}
Vector2i Entity::GetPosition( ) const
{
	return _position;
}