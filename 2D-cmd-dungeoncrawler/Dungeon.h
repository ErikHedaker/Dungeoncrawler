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

class Dungeon
{
	public:
		Dungeon( Player* player );

		void Build( char gameType );
		void GameLoop( );

		/* Used in Output::Dungeon */
		Entity* const GetEntityDataAt( const Vector2i& position ) const;
		Entity* const GetHiddenDataAt( const Vector2i& position ) const;
		bool GetVisionDataAt( const Vector2i& position ) const;

	private:
		GameState _state;
		Vector2i _dungeonSize;
		int _lineOfSight;

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

		void UpdateEntityDataAt( const Vector2i& position, Entity* entity );
		void UpdateHiddenDataAt( const Vector2i& position, Entity* entity );
		void UpdateVisionData( );

		/* Configuration */
		void SetDungeonSize( char gameType );
		void SetDungeonLineOfSight( char gameType );
		void SetDungeonMonsterAmount( char gameType );

		/* Resize containers */
		void BuildEntityData( );
		void BuildHiddenData( );
		void BuildVisionData( );

		/* Build Dungeon */
		void SetPlayer( );
		void SetExits( );
		void SetOuterWalls( );
		void SetHiddenPath( );
		void SetRandomSourceWalls( );
		void SetRandomExtensionWalls( );
		void SetFillerWalls( );
		void SetRandomMonsterPositions( );

		/* GameLoop */
		void PlayerTurn( char choice );
		void PlayerMovement( const Orientation& orientation );
		void RandomMonsterMovement( );
		void UpdateCharacters( );
		bool CheckGameState( ) const;
};