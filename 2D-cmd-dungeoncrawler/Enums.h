#pragma once

#include <map>
#include <string>

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
		None           = 0,
		MovementRandom = 1,
		Monster        = 1 << 1,
		Mortal         = 1 << 2,
		Hidden         = 1 << 3,
		WalkablePlayer = 1 << 4,
		WalkableOthers = 1 << 5
	};
};

struct Spells
{
	enum SpellsType
	{
		None     = 0,
		Fireball = 1,
		Iceblast = 1 << 1
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