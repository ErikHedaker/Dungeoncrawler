#pragma once

struct Icon
{
    enum IconType
    {
        Player  = '@',
        Monster = 'M',
        Door    = '+',
        Wall    = '#',
        Ground  = '-'
    };
};

struct Attributes
{
    enum AttributesType
    {
        PassablePlayer = 1 << 0,
        PassableOthers = 1 << 1,
        MovementRandom = 1 << 2,
        Monster        = 1 << 3,
        Hidden         = 1 << 4
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

enum class PlayerStatus
{
    Wandering,
    Traveling,
    Combat,
    Dead
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