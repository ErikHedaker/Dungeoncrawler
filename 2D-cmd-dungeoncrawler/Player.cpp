#include "Player.h"

Player::Player( int health, int damage, int abilities ) :
	Combatant( "Player", health, damage, abilities ),
	visionReach( 5 ),
	position( { -1, -1 } ),
	status( PlayerStatus::Traveling )
{ }