#pragma once

struct Category
{
    enum CategoryType
    {
        Ability      = 0,
        Character    = 1,
        Structure    = 2,
        PlayerEntity = 3
    };
};

struct Attributes
{
    enum AttributesType
    {
        PassablePlayer = 1 << 0,
        PassableOthers = 1 << 1,
        MovementRandom = 1 << 2,
        Combative      = 1 << 3
    };
};

struct Spells
{
    enum SpellsType
    {
        TouchOfDeath = 1 << 0,
        Fireball     = 1 << 1,
        Iceblast     = 1 << 2
    };
};

struct States
{
    enum StatesType
    {
        Switch = 1 << 0,
        Combat = 1 << 1,
        Dead   = 1 << 2,
        Loot   = 1 << 3
    };
};

struct Load
{
    enum LoadType
    {
        Default  = 0,
        Previous = 1
    };
};

enum class GameStatus
{
    Neutral,
    Menu
};

enum class GameConfig
{
    Default,
    Configure
};

enum class Orientation
{
    North,
    West,
    South,
    East
};

