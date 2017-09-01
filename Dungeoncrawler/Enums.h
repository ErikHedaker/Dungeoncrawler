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
        Combat = 1 << 0,
        Switch = 1 << 1,
        Loot   = 1 << 2
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

struct Load
{
    enum Enum
    {
        Default  = 0,
        Previous = 1
    };
};

enum class GameStatus
{
    Menu,
    Playing
};

enum class GameConfig
{
    Default,
    Configure
};

enum class ItemType
{
    Weapon,
    Armour
};