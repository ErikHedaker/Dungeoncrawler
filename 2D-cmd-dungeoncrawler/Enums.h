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

struct Attribute
{
	enum AttributeType
	{
		None           = 0,
		MovementRandom = 1,
		Monster        = 1 << 1,
		Mortal         = 1 << 2,
		Hidden         = 1 << 3,
		WalkablePlayer = 1 << 4,
		WalkableOthers = 1 << 5
	};
};

struct Ability
{
	enum AbilityType
	{
		None     = 0,
		Fireball = 1,
		IceBlast = 1 << 1
	};
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

enum class PlayerStatus
{
	Wandering,
	Traveling,
	Combat,
	Dead
};

enum class GameStatus
{
	Menu,
	Gameloop
};