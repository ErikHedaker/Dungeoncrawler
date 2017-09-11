#pragma once

struct EntityType
{
    enum Enum
    {
        Ability      = 0,
        Character    = 1,
        Structure    = 2
    };
};

struct Attributes
{
    enum Enum
    {
        AllowPlayer = 1 << 0,
        Obstacle    = 1 << 1,
        Movement    = 1 << 2,
        Combative   = 1 << 3,
        Lootable    = 1 << 4
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