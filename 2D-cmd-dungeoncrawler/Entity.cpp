#include "Entity.h"

Entity::Entity( const Vector2i& position, const Portrait& portrait ) :
	_position( position ),
	portrait( portrait )
{ }

void Entity::SetPosition( const Vector2i& position )
{
	_position = position;
}
const Vector2i Entity::GetPosition( ) const
{
	return _position;
}