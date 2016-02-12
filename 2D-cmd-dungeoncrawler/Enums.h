#pragma once

struct Icon
{
	enum IconType
	{
		Player  = '@',
		Monster = 'M',
		Door    = '+',
		Wall    = '#',
		Path    = ':',
		Ground  = '-'
	};
};

struct Attribute
{
	enum AttributeType
	{
		MovementRandom = 1,
		Aggressive     = 1 << 1,
		Mortal         = 1 << 2,
		Hidden         = 1 << 3,
		WalkablePlayer = 1 << 4,
		WalkableOthers = 1 << 5
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
	South,
	East,
	West
};

enum class GameStatus
{
	Neutral,
	Menu,
	Next
};