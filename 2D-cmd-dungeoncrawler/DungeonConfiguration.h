#pragma once

#include "Vector2i.h"

struct DungeonConfiguration
{
	bool fixedSeed = false;
	int seed = 0;

	void SetFixedSeed( );
	void SetSeed( );

	bool fixedDungeonSize = false;
	Vector2i dungeonSize = { 0, 0 };

	void SetFixedDungeonSize( );
	void SetDungeonSize( );

	bool generateDoors = true;
	bool generateOuterObstacles = true;
	bool generatePath = true;
	bool generateSourceObstacles = true;
	bool generateExtensionObstacles = true;
	bool generateFillerObstacles = true;
	bool generateMonsters = true;

	void SetGenerateDoors( );
	void SetGenerateOuterObstacles( );
	void SetGeneratePath( );
	void SetGenerateSourceObstacles( );
	void SetGenerateExtensionObstacles( );
	void SetGenerateFillerObstacles( );
	void SetGenerateMonsters( );

	int amountDoors = 0;
	int amountSourceObstacles = 0;
	int amountExtensionObstacles = 0;
	int amountFillerObstacleCycles = 0;
	int amountMonsters = 0;

	void SetAmountDoors( );
	void SetAmountSourceObstacles( );
	void SetAmountExtensionObstacles( );
	void SetAmountFillerObstacleCycles( );
	void SetAmountMonsters( );
};