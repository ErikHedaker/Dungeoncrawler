#include "Player.h"
#include "Enums.h"

Player::Player( const Vector2i& position ) :
	Character( position, EntityType::Player, 10.0f, 0.01f, 10.0f, 100.0f, 100.0f ),
	_visionReach( 4 )
{ }
Player::Player( const Vector2i& position, float speed, float armor, float damage, float health, float mana, int lineOfSight ) :
	Character( position, EntityType::Player, speed, armor, damage, health, mana ),
	_visionReach( lineOfSight )
{ }

int Player::GetVisionReach( ) const
{
	return _visionReach;
}
void Player::SetVisionReach( int value )
{
	_visionReach = value;
}