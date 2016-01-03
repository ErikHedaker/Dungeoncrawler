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

enum class GameState
{
	Neutral,
	Menu,
	Lost,
	Won
};

enum class Orientation
{
	North,
	South,
	East,
	West
};