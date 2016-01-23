#include "Monster.h"
#include "Enums.h"

Monster::Monster( const Vector2i& position ) :
	Character( position, EntityType::Monster, 10.0f, 0.10f, 10.0f, 100.0f, 100.0f )
{ }
Monster::Monster( const Vector2i& position, float speed, float armor, float damage, float health, float mana ) :
	Character( position, EntityType::Monster, speed, armor, damage, health, mana )
{ }