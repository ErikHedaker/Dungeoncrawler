#pragma once

#include "Enums.h"
#include "BattleSystem.h"
#include "Player.h"
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

struct Link
{
	std::size_t indexDungeon;
	Vector2i exit;
	Vector2i entry;
};

struct Components
{
	std::size_t indexCount = 0;
	std::vector<char> icon;
	std::vector<Vector2i> position;
	std::vector<Vector2i> positionPrevious;
	std::vector<int> attributes;

	std::size_t Add( );
	void Delete( std::size_t index );
};

struct Tile
{
	std::vector<std::size_t> indexOccupants;
	char icon = Icon::Ground;
};

class Dungeon
{
	public:
		Dungeon( const DungeonConfiguration& config );
		Dungeon( int maxCol, int maxRow, const std::vector<bool>& visionMap, const std::vector<char>& iconMap, Player& player );

		std::vector<Link> links;

		void CreatePlayerLocal( const Vector2i& position, Player& player );
		void RotateDungeonClockwise( );

		const std::pair<int, int> GetSize( ) const;
		const Tile& GetTile( const Vector2i& position ) const;
		bool GetVision( const Vector2i& position ) const;

		/* Game loop */
		void PlayerMovement( const Orientation& orientation );
		void RandomMovement( );
		void CheckEvents( Player& player );

		/* Helper functions */
		bool CheckTile( const Vector2i& position, int bitmask ) const;
		bool InBounds( const Vector2i& position ) const;
		bool IsCorner( const Vector2i& position ) const;
		bool Unoccupied( const Vector2i& position ) const;
		bool Surrounded( const Vector2i& position, int threshold ) const;

	private:
		int _maxCol;
		int _maxRow;
		Components _components;
		std::size_t _indexPlayerLocal;

		/* 1D arrays interpreted as 2D arrays */
		std::vector<Tile> _tileMap;
		std::vector<bool> _visionMap;

		void UpdateVision( const Vector2i& position, int visionReach );
		void UpdateTile( const Vector2i& position );
		void OccupantAdd( std::size_t index );
		void OccupantRemove( std::size_t index );
		void DeleteEntity( std::size_t index );

		/* Preset entities */
		void DoorAdd( const Vector2i& position );
		void WallAdd( const Vector2i& position );
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