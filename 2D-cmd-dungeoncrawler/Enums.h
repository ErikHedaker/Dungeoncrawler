#pragma once

/*
enum class Portrait : char
{
	Player = '@',
	Monster = 'M',
	Wall = '#',
	Door = '=',
	Step = ':',
	Floor = '-'
};
*/

struct Portrait
{
	static const char Player = '@';
	static const char Monster = 'M';
	static const char Wall = '#';
	static const char Door = '+';
	static const char Step = '=';
	static const char Ground = '-';
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