#pragma once

#include "BattleSystem.h"
#include "Vector2.h"
#include "Enums.h"

struct Player : public Combatant
{
    Player( int health, int healthMax, int healthRegen, int damage, int spells = 0 );

    int visionReach;
    Vector2<int> position;
    PlayerStatus status;
};