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
		MovementRandom = 1 << 0,
		Monster        = 1 << 1,
		Hidden         = 1 << 2,
		WalkablePlayer = 1 << 3,
		WalkableOthers = 1 << 4
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
	Neutral,
	Menu
};