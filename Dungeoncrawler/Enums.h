#pragma once

struct Attributes
{
    enum Enum
    {
        Obstacle    = 1 << 0,
        Movement    = 1 << 1,
        Hostile     = 1 << 2
    };
};

struct Spells
{
    enum Enum
    {
        TouchOfDeath = 1 << 0,
        Fireball     = 1 << 1,
        Iceblast     = 1 << 2
    };
};

struct States
{
    enum Enum
    {
        Fighting = 1 << 0,
        Swapping = 1 << 1,
        Looting  = 1 << 2
    };
};

struct Orientation
{
    enum Enum
    {
        North =  0,
        East  =  1,
        South =  2,
        West  = -1
    };
};

enum class ItemType
{
    Weapon,
    Armour
};