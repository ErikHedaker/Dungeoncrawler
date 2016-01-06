#pragma once

#include "Player.h"
#include "Monster.h"
#include "Wall.h"
#include "Exit.h"
#include "Path.h"
#include "Enums.h"
#include <vector>
#include <list>
#include <memory>
#include <fstream>

class Dungeon
{
	public:
		Dungeon( Player* player );

		void BuildDungeon( const GameType& type );
		void GameLoop( );

		void SaveDungeon( const std::string& fileName );
		void LoadDungeon( const std::string& fileName );

		/* Used in Output::Dungeon */
		const Entity* const GetEntityDataAt( const Vector2i& position ) const;
		const Entity* const GetHiddenDataAt( const Vector2i& position ) const;
		bool GetVisionDataAt( const Vector2i& position ) const;

		/* Helper functions */
		bool InBounds( const Vector2i& position ) const;
		bool FloorSurroundedWalls( const Vector2i& position, int threshold ) const;

	private:
		GameStatus _status;
		Vector2i _dungeonSize;

		/* All types inherit from Entity */
		std::list<Monster> _monsters;		// std::list guarantees unchanged
		std::list<Wall> _walls;				// element addresses as the
		std::list<Exit> _exits;				// container grows, unlike std::vector.
		std::list<Path> _paths;
		Player* _player;					// Non-owning pointer, Player exist outside of Dungeon.

		/* 1D arrays interpreted as 2D space */
		std::vector<Entity*> _entityData;	// Non-owning pointers, points to _monster, _walls, _exits and _player.
		std::vector<Entity*> _hiddenData;	// Non-owning pointers, points to _paths.
		std::vector<bool> _visionData;

		void SetEntityDataAt( const Vector2i& position, Entity* entity );
		void SetHiddenDataAt( const Vector2i& position, Entity* entity );
		void SetVisionDataAt( const Vector2i& position, bool vision );
		void UpdateVisionDataAt( const Vector2i& position, int lineOfSight );

		/* Configuration */
		void SetDungeonSize( const GameType& type );
		void SetPlayerLineOfSight( const GameType& type );
		void SetMonsterAmount( const GameType& type );

		/* Resize containers */
		void ResizeEntityData( );
		void ResizeHiddenData( );
		void ResizeVisionData( );

		/* Build Dungeon */
		void SetRandomPlayerPosition( );
		void SetRandomExits( );
		void SetOuterWalls( );
		void SetHiddenPath( );
		void SetRandomSourceWalls( );
		void SetRandomExtensionWalls( );
		void SetFillerWalls( );
		void SetRandomMonsterPositions( );

		/* Write to save file */
		void WriteDungeonSize( std::ofstream& stream );
		void WriteEntityData( std::ofstream& stream );
		void WriteHiddenData( std::ofstream& stream );
		void WriteVisionData( std::ofstream& stream );

		/* Read from save file */
		void ReadDungeonSize( std::ifstream& stream );
		void ReadEntityData( std::ifstream& stream );
		void ReadHiddenData( std::ifstream& stream );
		void ReadVisionData( std::ifstream& stream );

		/* GameLoop */
		void PlayerTurn( char choice );
		void PlayerMovement( const Orientation& orientation );
		void RandomMonsterMovement( );
		void UpdateCharacters( );
		bool CheckGameStatus( ) const;
};