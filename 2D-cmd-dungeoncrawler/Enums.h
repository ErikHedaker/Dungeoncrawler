#pragma once

/*
enum class Portrait : char
{
	Player = '@',
	Monster = 'M',
	Wall = '#',
	Exit = '=',
	Path = ':',
	Floor = '-'
};
*/

struct Portrait
{
	static const char Player = '@';
	static const char Monster = 'M';
	static const char Wall = '#';
	static const char Exit = '=';
	static const char Path = ':';
	static const char Floor = '-';
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