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
		MovementRandom = 1,
		Monster        = 1 << 1,
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
	West,
	South,
	East
};

enum class PlayerStatus
{
	Wandering,
	Traveling,
	Inactive,
	Dead
};