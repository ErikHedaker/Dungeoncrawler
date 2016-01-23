#include "Entity.h"

Entity::Entity( const Vector2i& position, const EntityType& type ) :
	_position( position ),
	type( type )
{ }

void Entity::SetPosition( const Vector2i& position )
{
	_position = position;
}
const Vector2i Entity::GetPosition( ) const
{
	return _position;
}