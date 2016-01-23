#pragma once

struct Icon
{
	static const char Player = '@';
	static const char Monster = 'M';
	static const char Door = '+';
	static const char Obstacle = '#';
	static const char Path = ':';
	static const char Ground = '-';
};

enum class EntityType
{
	Player,
	Monster,
	Door,
	Obstacle,
	Path
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
	Next,
	Dead
};

enum class DungeonType
{
	Randomization,
	Configuration
};