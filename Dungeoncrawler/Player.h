#pragma once

#include "BattleSystem.h"
#include "Vector2.h"
#include "Enums.h"

struct Player : public Combatant
{
    Player( int health, int healthMax, int healthRegeneration, int damage, int abilities );

    int visionReach;
    Vector2<int> position;
    PlayerStatus status;
};