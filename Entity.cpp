#include "Entity.h"

Entity::Entity( )
{

}
Entity::Entity( const Vector2i& position )
{
	_position.x = position.x;
	_position.y = position.y;
}

void Entity::SetPosition( Vector2i position )
{
	_position.x = position.x;
	_position.y = position.y;
}
Vector2i Entity::GetPosition( ) const
{
	return _position;
}