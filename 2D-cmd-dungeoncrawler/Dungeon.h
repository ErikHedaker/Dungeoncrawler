#pragma once

#include "Player.h"
#include "Monster.h"
#include "Enums.h"
#include "Functions.h"
#include "DungeonConfiguration.h"
#include <vector>
#include <list>
#include <memory>
#include <fstream>
#include <unordered_set>

struct Tile
{
	std::list<Entity*> occupants;
	char icon = '-';
};

class Dungeon
{
	public:
		void GenerateDungeon( );
		void GenerateDungeon( const DungeonConfiguration& config );

		//void SaveDungeon( const std::string& fileName ) const;
		//void LoadDungeon( const std::string& fileName );

		void PlayerAdd( Player* player, const Vector2i& position );
		void PlayerAdd( Player* player, Entity* door );

		void UpdatePlayerVision( );
		void PlayerMovement( const Orientation& orientation );
		void MonsterMovement( );
		void HandleEvents( GameStatus& status );
		void RemoveDeadCharacters( GameStatus& status, bool safe = true );

		void RotateDungeonClockwise( );
		void RotateEntityClockwise( Entity* entity );
		void UpdateEntityPositions( );

		const std::vector<Entity*> GetDoors( ) const;
		const Vector2i& GetSize( ) const;
		const Tile& GetTile( const Vector2i& position ) const;
		bool GetVision( const Vector2i& position ) const;

		/* Helper functions */
		bool Contains( const Vector2i& position, const EntityType& type ) const;
		bool Walkable( const Vector2i& position ) const;
		bool InBounds( const Vector2i& position ) const;
		bool Unoccupied( const Vector2i& position ) const;
		bool Surrounded( const Vector2i& position, int threshold ) const;

	private:
		bool _fixed;
		int _seed;
		Vector2i _dungeonSize;

		/* All types are or inherit from Entity */
		Player* _player;
		std::list<Monster> _monsters;
		std::list<Entity> _entities;

		/* 1D arrays interpreted as 2D grids */
		std::vector<Tile> _tileMap;
		std::vector<bool> _visionMap;

		/* Container modifiers */
		void UpdateVision( const Vector2i& position, int lineOfSight );
		void UpdateTile( const Vector2i& position );
		void OccupantAdd( Entity* entity );
		void OccupantRemove( Entity* entity );

		/* Generate Dungeon */
		void SetDungeonSize( bool set = false, const Vector2i& size = { 0, 0 } );
		void SetDungeonContainers( );
		void GenerateDoors( bool generate = true, int amount = 0 );
		void GenerateOuterObstacles( bool generate = true );
		void GeneratePath( bool generate = true );
		void GenerateSourceObstacles( bool generate = true, int amount = 0 );
		void GenerateExtensionObstacles( bool generate = true, int amount = 0 );
		void GenerateFillerObstacles( bool generate = true, int amount = 0 );
		void GenerateMonsters( bool generater = true, int amount = 0 );

		///* Write to save file */
		//void WriteDungeonSize( std::ofstream& stream ) const;
		//void WriteTileIcons( std::ofstream& stream ) const;
		//void WriteVision( std::ofstream& stream ) const;

		///* Read from save file */
		//void ReadDungeonSize( std::ifstream& stream );
		//void ReadTileIcons( std::ifstream& stream );
		//void ReadVision( std::ifstream& stream );
};