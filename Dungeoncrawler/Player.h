#pragma once

#include "BattleSystem.h"
#include "Vector2i.h"
#include "Enums.h"

struct Player : public Combatant
{
    Player( int health, int healthMax, int healthRegeneration, int damage, int abilities );

    int visionReach;
    Vector2i position;
    PlayerStatus status;
};