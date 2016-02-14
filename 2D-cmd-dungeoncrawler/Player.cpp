#include "Player.h"

Player::Player( ) :
	position( { -1, -1 } ),
	visionReach( 5 ),
	status( PlayerStatus::Traveling )
{ }