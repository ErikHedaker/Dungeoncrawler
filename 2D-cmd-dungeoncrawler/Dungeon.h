#pragma once

#include "Player.h"
#include "Monster.h"
#include "Enums.h"
#include "Functions.h"
#include <vector>
#include <list>
#include <utility>

struct DungeonConfiguration
{
	bool fixedDungeonSize = false;
	Vector2i dungeonSize = { 0, 0 };

	bool generateDoors = true;
	bool generateOuterObstacles = true;
	bool generatePath = true;
	bool generateSourceObstacles = true;
	bool generateExtensionObstacles = true;
	bool generateFillerObstacles = true;
	bool generateMonsters = true;

	int amountDoors = 0;
	int amountSourceObstacles = 0;
	int amountExtensionObstacles = 0;
	int amountFillerObstacleCycles = 0;
	int amountMonsters = 0;
};

struct Tile
{
	std::list<Entity*> occupants;
	char icon = '-';
};

class Dungeon
{
	public:
		Dungeon( Player* player, const DungeonConfiguration& config );
		Dungeon( Player* player, int maxCol, int maxRow, const std::vector<bool>& visionMap, const std::vector<char>& iconMap );

		void PlayerInitialPlace( const Vector2i& position );
		void PlayerInitialPlaceNearby( const Vector2i& position );

		void UpdatePlayerVision( );
		void PlayerMovement( const Orientation& orientation );
		void MonsterMovement( );
		void HandleEvents( GameStatus& status );
		void RemoveDeadCharacters( bool safe = true );

		void RotateDungeonClockwise( );
		void UpdateEntityPositions( );

		const std::vector<const Entity*> GetDoors( ) const;
		const std::pair<int, int> GetSize( ) const;
		const Tile& GetTile( const Vector2i& position ) const;
		bool GetVision( const Vector2i& position ) const;

		/* Helper functions */
		bool Contains( const Vector2i& position, const EntityType& type ) const;
		bool Walkable( const Vector2i& position ) const;
		bool InBounds( const Vector2i& position ) const;
		bool Unoccupied( const Vector2i& position ) const;
		bool Surrounded( const Vector2i& position, int threshold ) const;

	private:
		int _maxCol;
		int _maxRow;

		/* All types are or inherit from Entity */
		Player* _player;
		std::list<Monster> _monsters;
		std::list<Entity> _entities;

		/* 1D arrays interpreted as 2D grids */
		std::vector<Tile> _tileMap;
		std::vector<bool> _visionMap;

		void UpdateVision( const Vector2i& position, int lineOfSight );
		void UpdateTile( const Vector2i& position );
		void OccupantAdd( Entity* entity );
		void OccupantRemove( Entity* entity );

		/* Generate Dungeon */
		void GenerateDoors( bool generate, int amount );
		void GenerateOuterObstacles( bool generate );
		void GeneratePath( bool generate );
		void GenerateSourceObstacles( bool generate, int amount );
		void GenerateExtensionObstacles( bool generate, int amount );
		void GenerateFillerObstacles( bool generate, int amount );
		void GenerateMonsters( bool generater, int amount );
};