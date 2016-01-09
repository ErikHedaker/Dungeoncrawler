#include "Player.h"
#include "Enums.h"

Player::Player( const Vector2i& position ) :
	Character( position, Portrait::Player, 10.0f, 0.10f, 10.0f, 100.0f, 100.0f ),
	_lineOfSight( 5 )
{ }
Player::Player( const Vector2i& position, float speed, float armor, float damage, float health, float mana, int lineOfSight ) :
	Character( position, Portrait::Player, speed, armor, damage, health, mana ),
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