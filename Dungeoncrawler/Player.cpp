#include "Player.h"

Player::Player( int health, int healthMax, int healthRegen, int damage, int spells ) :
    Combatant( "Player", health, healthMax, healthRegen, damage, spells ),
    visionReach( 3 ),
    position( { -1, -1 } ),
    states( 0 )
{ }