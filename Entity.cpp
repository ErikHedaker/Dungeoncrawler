#pragma once

#include "Entity.h"

Entity::Entity( )
{

}
Entity::Entity( const int& x, const int &y )
{
	_position.x = x;
	_position.y = y;
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