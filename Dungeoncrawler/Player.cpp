#include "Player.h"

Player::Player( int health, int healthMax, int healthRegeneration, int damage, int spells ) :
    Combatant( "Player", health, healthMax, healthRegeneration, damage, spells ),
    visionReach( 3 ),
    position( { -1, -1 } ),
    status( PlayerStatus::Traveling )
{ }