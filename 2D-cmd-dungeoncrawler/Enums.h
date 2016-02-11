#pragma once

struct Icon
{
	static const char Player = '@';
	static const char Monster = 'M';
	static const char Door = '+';
	static const char Wall = '#';
	static const char Path = ':';
	static const char Ground = '-';
};

namespace Attribute
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
}

enum class ConfigType
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