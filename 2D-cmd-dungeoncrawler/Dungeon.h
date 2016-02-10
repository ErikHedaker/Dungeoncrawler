#pragma once

#include "Enums.h"
#include "Functions.h"
#include <vector>
#include <list>
#include <utility>

struct DungeonConfiguration
{
	bool fixedDungeonSize = false;
	int maxCol = 0;
	int maxRow = 0;

	bool generateDoors = true;
	bool generateOuterWalls = true;
	bool generatePath = true;
	bool generateSourceWalls = true;
	bool generateExtensionWalls = true;
	bool generateFillerWalls = true;
	bool generateMonsters = true;

	int amountDoors = 0;
	int amountSourceWalls = 0;
	int amountExtensionWalls = 0;
	int amountFillerWallsCycles = 0;
	int amountMonsters = 0;
};

struct Properties
{
	bool movementRandom;
	bool aggressive;
	bool mortal;
	bool hidden;
	bool walkablePlayer;
	bool walkableOthers;
};

struct Components
{
	std::size_t size = 0;
	std::vector<bool> active;
	std::vector<Properties> properties;
	std::vector<Vector2i> position;
	std::vector<Vector2i> positionPrevious;
	std::vector<int> health;
	std::vector<int> damage;
	std::vector<int> visionReach;
	std::vector<int> iconPriority;
	std::vector<char> icon;

	std::size_t Add( );
};

struct Tile
{
	std::vector<std::size_t> indexOccupants;
	char icon = '-';
};

class Dungeon
{
	public:
		Dungeon( const DungeonConfiguration& config );
		Dungeon( int maxCol, int maxRow, const std::vector<bool>& visionMap, const std::vector<char>& iconMap );

		void SetPositionPlayer( const Vector2i& position );

		void PrintDungeonCentered( const Vector2i& screenSize = { 40, 20 } );
		void PlayerMovement( const Orientation& orientation );
		void MonsterMovement( );
		void HandleEvents( GameStatus& status );

		void RotateDungeonClockwise( );
		void UpdateEntityPositions( );

		const std::vector<Vector2i> GetDoors( ) const;
		const std::pair<int, int> GetSize( ) const;
		const Tile& GetTile( const Vector2i& position ) const;
		bool GetVision( const Vector2i& position ) const;

		/* Helper functions */
		bool WalkablePlayer( const Vector2i& position ) const;
		bool WalkableOthers( const Vector2i& position ) const;
		bool InBounds( const Vector2i& position ) const;
		bool IsCorner( const Vector2i& position ) const;
		bool Unoccupied( const Vector2i& position ) const;
		bool Surrounded( const Vector2i& position, int threshold ) const;

	private:
		int _maxCol;
		int _maxRow;
		Components _components;
		std::vector<const std::size_t> _indexDoors;

		/* 1D arrays interpreted as 2D grids */
		std::vector<Tile> _tileMap;
		std::vector<bool> _visionMap;

		void UpdateVision( const Vector2i& position, int visionReach );
		void UpdateTile( const Vector2i& position );
		void OccupantAdd( std::size_t index );
		void OccupantRemove( std::size_t index );

		/* Preset entities */
		void PlayerAdd( const Vector2i& position );
		void WallAdd( const Vector2i& position );
		void DoorAdd( const Vector2i& position );
		void StepAdd( const Vector2i& position );
		void MonsterAdd( const Vector2i& position );

		/* Called in constructor */
		void GenerateDoors( bool generate, int amount );
		void GenerateOuterWalls( bool generate );
		void GeneratePath( bool generate );
		void GenerateSourceWalls( bool generate, int amount );
		void GenerateExtensionWalls( bool generate, int amount );
		void GenerateFillerWalls( bool generate, int amount );
		void GenerateMonsters( bool generater, int amount );
};