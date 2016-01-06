#pragma once

enum class Portrait : char
{
	Player = '@',
	Monster = 'M',
	Wall = '#',
	Exit = '=',
	Path = ':',
	Floor = '-'
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
	Lost,
	Won
};

enum class GameType
{
	Randomized,
	MinorConfiguration,
	MajorConfiguration
};