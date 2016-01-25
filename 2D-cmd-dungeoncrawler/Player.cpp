#include "Player.h"
#include "Enums.h"

Player::Player( const Vector2i& position ) :
	Character( position, EntityType::Player, 10.0f, 0.01f, 10.0f, 100.0f, 100.0f ),
	_lineOfSight( 3 )
{ }
Player::Player( const Vector2i& position, float speed, float armor, float damage, float health, float mana, int lineOfSight ) :
	Character( position, EntityType::Player, speed, armor, damage, health, mana ),
	_lineOfSight( lineOfSight )
{ }

int Player::GetLineOfSight( ) const
{
	return _lineOfSight;
}
void Player::SetLineOfSight( int value )
{
	_lineOfSight = value;
}