#pragma once

#include "EntityLibrary.h"
#include "Vector2.h"
#include "Enums.h"

struct Player : public Character
{
    Vector2<int> position;
    int visionReach = 4;
    int states = 0;
};