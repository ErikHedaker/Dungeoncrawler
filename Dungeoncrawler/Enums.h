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
        PassablePlayer = 1 << 0,
        PassableOthers = 1 << 1,
        MovementRandom = 1 << 2,
        Combative      = 1 << 3,
        Lootable       = 1 << 4
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

struct View
{
    enum Enum
    {
        Shrouded  = 0,
        Observed  = 1,
        Observing = 2
    };
};

enum class ItemType
{
    Weapon,
    Armour
};