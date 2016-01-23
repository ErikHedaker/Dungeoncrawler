#include "DungeonConfiguration.h"
#include "Functions.h"
#include <vector>

void DungeonConfiguration::SetFixedSeed( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = [] ( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	fixedSeed = GetBool( InputValidChar( "\nFixed seed for RandomNumberGenerator, [Y/N]: ", YesNo ) );
}
void DungeonConfiguration::SetSeed( )
{
	seed = InputPositiveInteger( "Enter seed: " );
}

void DungeonConfiguration::SetFixedDungeonSize( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = []( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	fixedDungeonSize = GetBool( InputValidChar( "\nFixed dungeon size, [Y/N]: ", YesNo ) );
}
void DungeonConfiguration::SetDungeonSize( )
{
	dungeonSize.col = InputPositiveInteger( "Enter dungeon col size: " );
	dungeonSize.row = InputPositiveInteger( "Enter dungeon col size: " );
}

void DungeonConfiguration::SetGenerateDoors( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = [] ( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	generateDoors = GetBool( InputValidChar( "\nGenerate doors, [Y/N]: ", YesNo ) );
}
void DungeonConfiguration::SetGenerateOuterObstacles( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = [] ( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	generateOuterObstacles = GetBool( InputValidChar( "\nGenerate outer obstacles, [Y/N]: ", YesNo ) );
}
void DungeonConfiguration::SetGeneratePath( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = [] ( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	generatePath = GetBool( InputValidChar( "\nGenerate path, [Y/N]: ", YesNo ) );
}
void DungeonConfiguration::SetGenerateSourceObstacles( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = [] ( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	generateSourceObstacles = GetBool( InputValidChar( "\nGenerate source obstacles, [Y/N]: ", YesNo ) );
}
void DungeonConfiguration::SetGenerateExtensionObstacles( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = [] ( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	generateExtensionObstacles = GetBool( InputValidChar( "\nGenerate extension, [Y/N]: ", YesNo ) );
}
void DungeonConfiguration::SetGenerateFillerObstacles( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = [] ( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	generateFillerObstacles = GetBool( InputValidChar( "\nGenerate filler obstacles, [Y/N]: ", YesNo ) );
}
void DungeonConfiguration::SetGenerateMonsters( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = [] ( char input ) -> bool
	{
		return input == 'Y' || input == 'y';
	};

	generateMonsters = GetBool( InputValidChar( "\nGenerate monsters, [Y/N]: ", YesNo ) );
}

void DungeonConfiguration::SetAmountDoors( )
{
	amountDoors = InputPositiveInteger( "Enter amount of doors: " );
}
void DungeonConfiguration::SetAmountSourceObstacles( )
{
	amountSourceObstacles = InputPositiveInteger( "Enter amount of source obstacles: " );
}
void DungeonConfiguration::SetAmountExtensionObstacles( )
{
	amountExtensionObstacles = InputPositiveInteger( "Enter amount of extension obstacles: " );
}
void DungeonConfiguration::SetAmountFillerObstacleCycles( )
{
	amountFillerObstacleCycles = InputPositiveInteger( "Enter amount of filler obstacle cycles: " );
}
void DungeonConfiguration::SetAmountMonsters( )
{
	amountMonsters = InputPositiveInteger( "Enter amount of monsters: " );
}